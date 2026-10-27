"""Generate an original, filesystem-free 68000 diagnostic boot disk.

No ROM or third-party program bytes are included. The boot program polls real
Amiga input registers and publishes a small result structure at chip RAM $70000.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path

RAM = 0x70000
MAGIC = 0x47544931  # GTI1


class Program:
    def __init__(self):
        self.code = bytearray()
        self.labels = {}
        self.fixups = []
        self.listing = []

    def emit(self, text, *words):
        self.listing.append(f"{12 + len(self.code):04x}: {text}")
        for word in words:
            self.code += struct.pack('>H', word & 0xffff)

    def label(self, name):
        self.labels[name] = len(self.code)
        self.listing.append(name + ':')

    def branch(self, condition, label):
        self.emit(f'{condition}.w {label}', {'bra': 0x6000, 'bne': 0x6600, 'beq': 0x6700}[condition], 0)
        self.fixups.append((len(self.code) - 2, label))

    def finish(self):
        for offset, label in self.fixups:
            struct.pack_into('>h', self.code, offset, self.labels[label] - offset)
        return bytes(self.code)


def make_disk(destination):
    p = Program()
    p.emit('lea $dff000,a6', 0x4df9, 0x00df, 0xf000)
    for reg in [0x96, 0x9a, 0x9c]:
        p.emit(f'move.w #$7fff,${reg:x}(a6)', 0x3d7c, 0x7fff, reg)
    p.emit('move.w #$0200,$100(a6)', 0x3d7c, 0x0200, 0x100)
    p.emit('lea $70000,a0', 0x41f9, 0x0007, 0x0000)
    p.emit('cmpi.l #GTI1,(a0)', 0x0c90, MAGIC >> 16, MAGIC)
    p.branch('beq', 'existing')
    for offset in range(4, 64, 4):
        p.emit(f'clr.l {offset}(a0)', 0x42a8, offset)
    p.label('existing')
    p.emit('addq.l #1,4(a0) ; boot count', 0x52a8, 4)
    p.emit('clr.l 8(a0) ; VPOS frame count', 0x42a8, 8)
    p.emit('moveq #0,d2 ; CPU register clock independent of RAM', 0x7400)
    p.emit('move.l $f8000c,56(a0) ; executing ROM version/revision', 0x2179, 0x00f8, 0x000c, 56)
    p.emit('move.l #$248,28(a0) ; guest colour/state marker', 0x217c, 0, 0x248, 28)
    p.emit('move.l #GTI1,(a0)', 0x20bc, MAGIC >> 16, MAGIC)
    p.emit('andi.b #$bf,$bfee01 ; keyboard serial input', 0x0239, 0x00bf, 0x00bf, 0xee01)
    p.label('away')
    p.emit('move.w $006(a6),d0', 0x302e, 6)
    p.emit('andi.w #$ff00,d0', 0x0240, 0xff00)
    p.emit('cmpi.w #$ff00,d0', 0x0c40, 0xff00)
    p.branch('beq', 'away')
    p.label('wait')
    p.emit('move.w $006(a6),d0', 0x302e, 6)
    p.emit('andi.w #$ff00,d0', 0x0240, 0xff00)
    p.emit('cmpi.w #$ff00,d0', 0x0c40, 0xff00)
    p.branch('bne', 'wait')
    p.emit('addq.l #1,8(a0)', 0x52a8, 8)
    p.emit('addq.l #1,d2', 0x5282)
    p.emit('move.l d2,52(a0) ; register witness', 0x2142, 52)
    p.emit('move.w $00a(a6),12(a0) ; JOY0DAT', 0x316e, 0x000a, 12)
    p.emit('move.w $00c(a6),14(a0) ; JOY1DAT', 0x316e, 0x000c, 14)
    p.emit('move.b $bfe001,16(a0) ; CIAA PRA buttons', 0x1179, 0x00bf, 0xe001, 16)
    p.emit('move.w $016(a6),18(a0) ; POTGOR', 0x316e, 0x0016, 18)
    p.emit('move.b $bfed01,d0 ; CIAA serial IRQ', 0x1039, 0x00bf, 0xed01)
    p.emit('btst #3,d0', 0x0800, 3)
    p.branch('beq', 'display')
    p.emit('move.b $bfec01,d0 ; keyboard serial data', 0x1039, 0x00bf, 0xec01)
    p.emit('move.b d0,20(a0)', 0x1140, 20)
    p.emit('move.l 24(a0),d1', 0x2228, 24)
    p.emit('andi.w #15,d1', 0x0241, 15)
    p.emit('move.b d0,32(a0,d1.w) ; serial event ring', 0x1180, 0x1020)
    p.emit('addq.l #1,24(a0)', 0x52a8, 24)
    p.emit('ori.b #$40,$bfee01 ; keyboard handshake', 0x0039, 0x0040, 0x00bf, 0xee01)
    p.emit('move.w #512,d1', 0x323c, 512)
    p.label('delay')
    p.emit('dbra d1,delay ; >85 us even on stock A1200', 0x51c9, 0)
    p.fixups.append((len(p.code) - 2, 'delay'))
    p.emit('andi.b #$bf,$bfee01', 0x0239, 0x00bf, 0x00bf, 0xee01)
    p.label('display')
    p.emit('move.w 30(a0),$180(a6) ; marker drives COLOR00', 0x3d68, 30, 0x180)
    p.branch('bra', 'away')
    code = p.finish()
    assert len(code) <= 1012
    disk = bytearray(1760 * 512)
    disk[:12] = struct.pack('>4sII', b'DOS\0', 0, 880)
    disk[12:12 + len(code)] = code
    checksum = 0
    for (value,) in struct.iter_unpack('>I', disk[:1024]):
        checksum += value
        checksum = (checksum & 0xffffffff) + (checksum >> 32)
    struct.pack_into('>I', disk, 4, (~checksum) & 0xffffffff)
    verify = 0
    for (value,) in struct.iter_unpack('>I', disk[:1024]):
        verify += value
        verify = (verify & 0xffffffff) + (verify >> 32)
    assert verify == 0xffffffff
    destination = Path(destination)
    destination.parent.mkdir(parents=True, exist_ok=True)
    if destination.exists():
        raise FileExistsError(destination)
    destination.write_bytes(disk)
    return {'path': str(destination), 'bytes': len(disk), 'sha256': hashlib.sha256(disk).hexdigest(),
            'boot_code_bytes': len(code), 'boot_checksum_valid': True,
            'result_address': hex(RAM), 'magic': hex(MAGIC), 'listing': p.listing}


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('destination')
    args = parser.parse_args()
    print(json.dumps(make_disk(args.destination), indent=2))
