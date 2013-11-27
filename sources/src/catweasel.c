/*
 * PUAE Catweasel support
 *
 * Copyright
 * Copyright 2011 Mustafa TUFAN
 *
 * some parts inspired or taken from cwfloppy
 * Copyright (C) 1998-2009 Michael Krause
 *
 */

#include <stdio.h>

#include "sysconfig.h"
#include "sysdeps.h"

#ifdef CATWEASEL

#include "options.h"
#include "memory_uae.h"
#include "ioport.h"
#include "catweasel.h"
#include "uae.h"
#include "zfile.h"

#define DRIVER
#include <catweasl_usr.h>

struct catweasel_contr cwc;

static int cwhsync, cwmk3buttonsync;
static int cwmk3port, cwmk3port1, cwmk3port2;
static int handshake;
static int mouse_x[2], mouse_y[2], mouse_px[2], mouse_py[2];

static HANDLE handle = INVALID_HANDLE_VALUE;

int catweasel_isjoystick (void)
{
	uae_u8 b = cwc.can_joy;
	if (!cwc.direct_access)
		return 0;
	if (b) {
		if (cwc.type == CATWEASEL_TYPE_MK3 && cwc.sid[0])
			b |= 0x80;
		if (cwc.type >= CATWEASEL_TYPE_MK4)
			b |= 0x80;
	}
	return b;
}
int catweasel_ismouse (void)
{
	if (!cwc.direct_access)
		return 0;
	return cwc.can_mouse;
}

static int hsync_requested;
static void hsync_request (void)
{
	hsync_requested = 10;
};

static void sid_write (uae_u8 reg, uae_u8 val, int sidnum)
{
	if (sidnum >= cwc.can_sid)
		return;
	catweasel_do_bput(0xd8, val);
	catweasel_do_bput(0xdc, reg | (sidnum << 7));
	catweasel_do_bget(0xd8); // dummy read
	catweasel_do_bget(0xd8); // dummy read
}

static uae_u8 sid_read (uae_u8 reg, int sidnum)
{
	if (sidnum >= cwc.can_sid)
		return 0;
	catweasel_do_bput(0xdc, 0x20 | reg | (sidnum << 7));
	catweasel_do_bget(0xd8); // dummy read
	catweasel_do_bget(0xd8); // dummy read
	return catweasel_do_bget(0xd8);
}

static uae_u8 get_buttons (void)
{
	uae_u8 b, b2;

	b = 0;
	if (cwc.type < CATWEASEL_TYPE_MK3 || !cwc.direct_access)
		return b;
	hsync_request();
	b2 = catweasel_do_bget(0xc8) & (0x80 | 0x40);
	if (!(b2 & 0x80))
		b |= 0x80;
	if (!(b2 & 0x40))
		b |= 0x08;
	if (cwc.type >= CATWEASEL_TYPE_MK4) {
		b &= ~0x80;
		catweasel_do_bput(3, 0x81);
		if (!(catweasel_do_bget(0x07) & 0x10))
			b |= 0x80;
		b2 = catweasel_do_bget(0xd0) ^ 15;
		catweasel_do_bput(3, 0x41);
		if (cwc.sid[0]) {
			b2 &= ~(1 | 2);
			if (sid_read(0x19, 0) > 0x7f)
				b2 |= 2;
			if (sid_read(0x1a, 0) > 0x7f)
				b2 |= 1;
		}
		if (cwc.sid[1]) {
			b2 &= ~(4 | 8);
			if (sid_read(0x19, 1) > 0x7f)
				b2 |= 8;
			if (sid_read(0x1a, 1) > 0x7f)
				b2 |= 4;
		}
	} else {
		b2 = cwmk3port1 | (cwmk3port2 << 2);
	}
	b |= (b2 & (8 | 4)) << 3;
	b |= (b2 & (1 | 2)) << 1;
	return b;
}

int catweasel_read_mouse (int port, int *dx, int *dy, int *buttons)
{
	if (!cwc.can_mouse || !cwc.direct_access)
		return 0;
	hsync_request();
	*dx = mouse_x[port];
	mouse_x[port] = 0;
	*dy = mouse_y[port];
	mouse_y[port] = 0;
	*buttons = (get_buttons() >> (port * 4)) & 15;
	return 1;
}

static void sid_reset (void)
{
	int i;
	for (i = 0; i < 0x19; i++) {
		sid_write(i, 0, 0);
		sid_write(i, 0, 1);
	}
}

static void catweasel_detect_sid (void)
{
	int i, j;
	uae_u8 b1, b2;

	cwc.sid[0] = cwc.sid[1] = 0;
	if (!cwc.can_sid || !cwc.direct_access)
		return;
	sid_reset();
	if (cwc.type >= CATWEASEL_TYPE_MK4) {
		catweasel_do_bput(3, 0x81);
		b1 = catweasel_do_bget(0xd0);
		for (i = 0; i < 100; i++) {
			sid_read(0x19, 0); // delay
			b2 = catweasel_do_bget(0xd0);
			if ((b1 & 3) != (b2 & 3))
				cwc.sid[0] = 6581;
			if ((b1 & 12) != (b2 & 12))
				cwc.sid[1] = 6581;
		}
	}
	catweasel_do_bput(3, 0x41);
	for (i = 0; i < 2 ;i++) {
		sid_reset();
		sid_write(0x0f, 0xff, i);
		sid_write(0x12, 0x10, i);
		for(j = 0; j != 1000; j++) {
			sid_write(0, 0, i);
			if((sid_read(0x1b, i) & 0x80) != 0) {
				cwc.sid[i] = 6581;
				break;
			}
		}
		sid_reset();
		sid_write(0x0f, 0xff, i);
		sid_write(0x12, 0x30, i);
		for(j = 0; j != 1000; j++) {
			sid_write(0, 0, i);
			if((sid_read(0x1b, i) & 0x80) != 0) {
				cwc.sid[i] = 8580;
				break;
			}
		}
	}
	sid_reset();
}

void catweasel_hsync (void)
{
	int i;

	if (cwc.type < CATWEASEL_TYPE_MK3 || !cwc.direct_access)
		return;
	cwhsync--;
	if (cwhsync > 0)
		return;
	cwhsync = 10;
	if (handshake) {
		/* keyboard handshake */
		catweasel_do_bput (0xd0, 0);
		handshake = 0;
	}
	if (hsync_requested < 0)
		return;
	hsync_requested--;
	if (cwc.type == CATWEASEL_TYPE_MK3 && cwc.sid[0]) {
		uae_u8 b;
		cwmk3buttonsync--;
		if (cwmk3buttonsync <= 0) {
			cwmk3buttonsync = 30;
			b = 0;
			if (sid_read (0x19, 0) > 0x7f)
				b |= 2;
			if (sid_read (0x1a, 0) > 0x7f)
				b |= 1;
			if (cwmk3port == 0) {
				cwmk3port1 = b;
				catweasel_do_bput (0xd4, 0); // select port2
				cwmk3port = 1;
			} else {
				cwmk3port2 = b;
				catweasel_do_bget (0xd4); // select port1
				cwmk3port = 0;
			}
		}
	}
	if (!cwc.can_mouse)
		return;
	/* read MK4 mouse counters */
	catweasel_do_bput (3, 0x81);
	for (i = 0; i < 2; i++) {
		int x, y, dx, dy;
		x = (uae_s8)catweasel_do_bget (0xc4 + i * 8);
		y = (uae_s8)catweasel_do_bget (0xc0 + i * 8);
		dx = mouse_px[i] - x;
		if (dx > 127)
			dx = 255 - dx;
		if (dx < -128)
			dx = 255 + dx;
		dy = mouse_py[i] - y;
		if (dy > 127)
			dy = 255 - dy;
		if (dy < -128)
			dy = 255 + dy;
		mouse_x[i] -= dx;
		mouse_y[i] -= dy;
		mouse_px[i] = x;
		mouse_py[i] = y;
	}
	catweasel_do_bput (3, 0x41);
}

int catweasel_read_joystick (uae_u8 *dir, uae_u8 *buttons)
{
	if (!cwc.can_joy || !cwc.direct_access)
		return 0;
	hsync_request ();
	*dir = catweasel_do_bget (0xc0);
	*buttons = get_buttons ();
	return 1;
}

int catweasel_read_keyboard (uae_u8 *keycode)
{
	uae_u8 v;

	if (!cwc.can_kb || !cwc.direct_access)
		return 0;
	if (!currprefs.catweasel)
		return 0;
	v = catweasel_do_bget (0xd4);
	if (!(v & 0x80))
		return 0;
	if (handshake)
		return 0;
	*keycode = catweasel_do_bget (0xd0);
	catweasel_do_bput (0xd0, 0);
	handshake = 1;
	return 1;
}

uae_u32	catweasel_do_bget (uaecptr addr)
{
	DWORD did_read = 0;
	uae_u8 buf1[1], buf2[1];

	if (addr >= 0x100)
		return 0;
	buf1[0] = (uae_u8)addr;
#if 0
	if (handle != INVALID_HANDLE_VALUE) {
		if (!DeviceIoControl (handle, CW_PEEKREG_FULL, buf1, 1, buf2, 1, &did_read, 0))
			write_log (_T("catweasel_do_bget %02x fail err=%d\n"), buf1[0], GetLastError ());
	} else {
#endif
		buf2[0] = ioport_read (cwc.iobase + addr);
#if 0
	}
	//write_log (_T("G %02X %02X %d\n"), buf1[0], buf2[0], did_read);
#endif
	return buf2[0];
}

void catweasel_do_bput (uaecptr	addr, uae_u32 b)
{
	if (addr >= 0x100)
		return;

	uae_u8 buf[2];
	DWORD did_read = 0;

	buf[0] = (uae_u8)addr;
	buf[1] = b;
#if 0
	if (handle != INVALID_HANDLE_VALUE) {
		if (!DeviceIoControl (handle, CW_POKEREG_FULL, buf, 2, 0, 0, &did_read, 0))
			write_log (_T("catweasel_do_bput %02x=%02x fail err=%d\n"), buf[0], buf[1], GetLastError ());
	} else {
#endif
		ioport_write (cwc.iobase + addr, b);
	}
	//write_log (_T("P %02X %02X %d\n"), (uae_u8)addr, (uae_u8)b, did_read);
}

#include "core.cw4.cpp"

static int cw_config_done (void)
{
	return ioport_read (cwc.iobase + 7) & 4;
}
static int cw_fpga_ready (void)
{
	return ioport_read (cwc.iobase + 7) & 8;
}
static void cw_resetFPGA (void)
{
	ioport_write (cwc.iobase + 2, 227);
	ioport_write (cwc.iobase + 3, 0);
	sleep_millis (10);
	ioport_write (cwc.iobase + 3, 65);
}

static int catweasel3_configure (void)
{
	ioport_write (cwc.iobase, 241);
	ioport_write (cwc.iobase + 1, 0);
	ioport_write (cwc.iobase + 2, 0);
	ioport_write (cwc.iobase + 4, 0);
	ioport_write (cwc.iobase + 5, 0);
	ioport_write (cwc.iobase + 0x29, 0);
	ioport_write (cwc.iobase + 0x2b, 0);
	return 1;
}

static int catweasel4_configure (void)
{
	struct zfile *f;
	time_t t;

	ioport_write (cwc.iobase, 241);
	ioport_write (cwc.iobase + 1, 0);
	ioport_write (cwc.iobase + 2, 227);
	ioport_write (cwc.iobase + 3, 65);
	ioport_write (cwc.iobase + 4, 0);
	ioport_write (cwc.iobase + 5, 0);
	ioport_write (cwc.iobase + 0x29, 0);
	ioport_write (cwc.iobase + 0x2b, 0);
	sleep_millis(10);

	if (cw_config_done()) {
		write_log (_T("CW: FPGA already configured, skipping core upload\n"));
		return 1;
	}
	cw_resetFPGA();
	sleep_millis(10);
	if (cw_config_done()) {
		write_log (_T("CW: FPGA failed to reset!\n"));
		return 0;
	}
	f = zfile_fopen(_T("core.cw4"), _T("rb"), ZFD_NORMAL);
	if (!f) {
		f = zfile_fopen_data (_T("core.cw4.gz"), core_len, core);
		f = zfile_gunzip (f, NULL);
	}
	write_log (_T("CW: starting core upload, this will take few seconds\n"));
	t = time(NULL) + 10; // give up if upload takes more than 10s
	for (;;) {
		uae_u8 b;
		if (zfile_fread (&b, 1, 1, f) != 1)
			break;
		ioport_write (cwc.iobase + 3, (b & 1) ? 67 : 65);
		while (!cw_fpga_ready()) {
			if (time(NULL) >= t) {
				write_log (_T("CW: FPGA core upload got stuck!?\n"));
				cw_resetFPGA();
				return 0;
			}
		}
		ioport_write (cwc.iobase + 192, b);
	}
	if (!cw_config_done()) {
		write_log (_T("CW: FPGA didn't accept the core!\n"));
		cw_resetFPGA();
		return 0;
	}
	sleep_millis(10);
	write_log (_T("CW: core uploaded successfully\n"));
	return 1;
}

static int detected;

void catweasel_free (void)
{
}

int catweasel_init(void)
{
fail:
	catweasel_free ();
	return 0;
}

int catweasel_detect (void)
{
//	static struct cw_controller_struct controllers[2];
	int err;

	err = pci_register_driver (&cwfloppy_pci_mk3);
	if (err && err != -ENODEV)
		return err;
	err = pci_register_driver (&cwfloppy_pci_mk4);
	if (err && err != -ENODEV)
		return err;
//	if (controllers[0].c.type == CATWEASEL_TYPE_NONE) {
//		write_log("No PCI Catweasels found.\n");
//	}

	pci_unregister_driver(&cwfloppy_pci_mk4);
	pci_unregister_driver(&cwfloppy_pci_mk3);

	if (detected)
		return detected < 0 ? 0 : 1;
}

static int cwfloppy_probe_mk3(struct pci_dev *pcidev, const struct pci_device_id *pciid)
{
	return 0;
}

static int cwfloppy_probe_mk4(struct pci_dev *pcidev, const struct pci_device_id *pciid)
{
	return 0;
}

#define CATWEASEL_TYPE_NONE  -1
#define CATWEASEL_TYPE_MK1    1
#define CATWEASEL_TYPE_MK3    3
#define CATWEASEL_TYPE_MK4    4

/* pci ids */
#define CW_MK4_VENDOR           0xe159
#define CW_MK4_DEVICE           0x0001

static struct pci_device_id id_table_mk3[] = {
	{ CW_MK4_VENDOR, CW_MK4_DEVICE, 0x1212, 0x0002, },
	{ 0, }
};
MODULE_DEVICE_TABLE(pci, id_table_mk3);
static struct pci_driver cwfloppy_pci_mk3 = {
	name: "cwfloppy_mk3",
	id_table: id_table_mk3,
	probe: cwfloppy_probe_mk3
};

static struct pci_device_id id_table_mk4[] = {
	/* The MK4 PCI bridge has a bug where the reported subvendor and
	 * subdevice IDs may randomly change between various values. */
	{ CW_MK4_VENDOR, CW_MK4_DEVICE, 0x5213, 0x0002, },
	{ CW_MK4_VENDOR, CW_MK4_DEVICE, 0x5213, 0x0003, },
	{ CW_MK4_VENDOR, CW_MK4_DEVICE, 0x5200, 0x0002, },
	{ CW_MK4_VENDOR, CW_MK4_DEVICE, 0x5200, 0x0003, },
	{ 0, }
};
MODULE_DEVICE_TABLE(pci, id_table_mk4);
static struct pci_driver cwfloppy_pci_mk4 = {
	.name = "cwfloppy_mk4",
	.id_table = id_table_mk4,
	.probe = cwfloppy_probe_mk4,
};

#endif
