/*
 * UAE - The Un*x Amiga Emulator
 *
 * Read 68000 CPU specs from file "table68k"
 *
 * Copyright 1995,1996 Bernd Schmidt
 */

#include "sysconfig.h"
#include <stdlib.h>
#include "uae_string.h"
#include "uae_types.h"

#define NO_MACHDEP 1
#include "sysdeps.h"

#include "writelog.h"
#include <ctype.h>

#include "readcpu.h"

#ifndef _T
# define _T
#endif // _T
/*
 * You can specify numbers from 0 to 5 here. It is possible that higher
 * numbers will make the CPU emulation slightly faster, but if the setting
 * is too high, you will run out of memory while compiling.
 * Best to leave this as it is.
 */
#ifndef CPU_EMU_SIZE
# define CPU_EMU_SIZE 0
#endif

int nr_cpuop_funcs;

struct mnemolookup lookuptab[] = {
	{ i_ILLG, _T("ILLEGAL"), NULL },
	{ i_OR, _T("OR"), NULL },
	{ i_CHK, _T("CHK"), NULL },
	{ i_CHK2, _T("CHK2"), NULL },
	{ i_AND, _T("AND"), NULL },
	{ i_EOR, _T("EOR"), NULL },
	{ i_ORSR, _T("ORSR"), NULL },
	{ i_ANDSR, _T("ANDSR"), NULL },
	{ i_EORSR, _T("EORSR"), NULL },
	{ i_SUB, _T("SUB"), NULL },
	{ i_SUBA, _T("SUBA"), NULL },
	{ i_SUBX, _T("SUBX"), NULL },
	{ i_SBCD, _T("SBCD"), NULL },
	{ i_ADD, _T("ADD"), NULL },
	{ i_ADDA, _T("ADDA"), NULL },
	{ i_ADDX, _T("ADDX"), NULL },
	{ i_ABCD, _T("ABCD"), NULL },
	{ i_NEG, _T("NEG"), NULL },
	{ i_NEGX, _T("NEGX"), NULL },
	{ i_NBCD, _T("NBCD"), NULL },
	{ i_CLR, _T("CLR"), NULL },
	{ i_NOT, _T("NOT"), NULL },
	{ i_TST, _T("TST"), NULL },
	{ i_BTST, _T("BTST"), NULL },
	{ i_BCHG, _T("BCHG"), NULL },
	{ i_BCLR, _T("BCLR"), NULL },
	{ i_BSET, _T("BSET"), NULL },
	{ i_CMP, _T("CMP"), NULL },
	{ i_CMPM, _T("CMPM"), NULL },
	{ i_CMPA, _T("CMPA"), NULL },
	{ i_MVPRM, _T("MVPRM"), NULL },
	{ i_MVPMR, _T("MVPMR"), NULL },
	{ i_MOVE, _T("MOVE"), NULL },
	{ i_MOVEA, _T("MOVEA"), NULL },
	{ i_MVSR2, _T("MVSR2"), NULL },
	{ i_MV2SR, _T("MV2SR"), NULL },
	{ i_SWAP, _T("SWAP"), NULL },
	{ i_EXG, _T("EXG"), NULL },
	{ i_EXT, _T("EXT"), NULL },
	{ i_MVMEL, _T("MVMEL"), _T("MOVEM") },
	{ i_MVMLE, _T("MVMLE"), _T("MOVEM") },
	{ i_TRAP, _T("TRAP"), NULL },
	{ i_MVR2USP, _T("MVR2USP"), NULL },
	{ i_MVUSP2R, _T("MVUSP2R"), NULL },
	{ i_NOP, _T("NOP"), NULL },
	{ i_RESET, _T("RESET"), NULL },
	{ i_RTE, _T("RTE"), NULL },
	{ i_RTD, _T("RTD"), NULL },
	{ i_LINK, _T("LINK"), NULL },
	{ i_UNLK, _T("UNLK"), NULL },
	{ i_RTS, _T("RTS"), NULL },
	{ i_STOP, _T("STOP"), NULL },
	{ i_TRAPV, _T("TRAPV"), NULL },
	{ i_RTR, _T("RTR"), NULL },
	{ i_JSR, _T("JSR"), NULL },
	{ i_JMP, _T("JMP"), NULL },
	{ i_BSR, _T("BSR"), NULL },
	{ i_Bcc, _T("Bcc"), NULL },
	{ i_LEA, _T("LEA"), NULL },
	{ i_PEA, _T("PEA"), NULL },
	{ i_DBcc, _T("DBcc"), NULL },
	{ i_Scc, _T("Scc"), NULL },
	{ i_DIVU, _T("DIVU"), NULL },
	{ i_DIVS, _T("DIVS"), NULL },
	{ i_MULU, _T("MULU"), NULL },
	{ i_MULS, _T("MULS"), NULL },
	{ i_ASR, _T("ASR"), NULL },
	{ i_ASL, _T("ASL"), NULL },
	{ i_LSR, _T("LSR"), NULL },
	{ i_LSL, _T("LSL"), NULL },
	{ i_ROL, _T("ROL"), NULL },
	{ i_ROR, _T("ROR"), NULL },
	{ i_ROXL, _T("ROXL"), NULL },
	{ i_ROXR, _T("ROXR"), NULL },
	{ i_ASRW, _T("ASRW"), NULL },
	{ i_ASLW, _T("ASLW"), NULL },
	{ i_LSRW, _T("LSRW"), NULL },
	{ i_LSLW, _T("LSLW"), NULL },
	{ i_ROLW, _T("ROLW"), NULL },
	{ i_RORW, _T("RORW"), NULL },
	{ i_ROXLW, _T("ROXLW"), NULL },
	{ i_ROXRW, _T("ROXRW"), NULL },

	{ i_MOVE2C, _T("MOVE2C"), _T("MOVEC") },
	{ i_MOVEC2, _T("MOVEC2"), _T("MOVEC") },
	{ i_CAS, _T("CAS"), NULL },
	{ i_CAS2, _T("CAS2"), NULL },
	{ i_MULL, _T("MULL"), NULL },
	{ i_DIVL, _T("DIVL"), NULL },
	{ i_BFTST, _T("BFTST"), NULL },
	{ i_BFEXTU, _T("BFEXTU"), NULL },
	{ i_BFCHG, _T("BFCHG"), NULL },
	{ i_BFEXTS, _T("BFEXTS"), NULL },
	{ i_BFCLR, _T("BFCLR"), NULL },
	{ i_BFFFO, _T("BFFFO"), NULL },
	{ i_BFSET, _T("BFSET"), NULL },
	{ i_BFINS, _T("BFINS"), NULL },
	{ i_PACK, _T("PACK"), NULL },
	{ i_UNPK, _T("UNPK"), NULL },
	{ i_TAS, _T("TAS"), NULL },
	{ i_BKPT, _T("BKPT"), NULL },
	{ i_CALLM, _T("CALLM"), NULL },
	{ i_RTM, _T("RTM"), NULL },
	{ i_TRAPcc, _T("TRAPcc"), NULL },
	{ i_MOVES, _T("MOVES"), NULL },
	{ i_FPP, _T("FPP"), NULL },
	{ i_FDBcc, _T("FDBcc"), NULL },
	{ i_FScc, _T("FScc"), NULL },
	{ i_FTRAPcc, _T("FTRAPcc"), NULL },
	{ i_FBcc, _T("FBcc"), NULL },
	{ i_FBcc, _T("FBcc"), NULL },
	{ i_FSAVE, _T("FSAVE"), NULL },
	{ i_FRESTORE, _T("FRESTORE"), NULL },

	{ i_CINVL, _T("CINVL"), NULL },
	{ i_CINVP, _T("CINVP"), NULL },
	{ i_CINVA, _T("CINVA"), NULL },
	{ i_CPUSHL, _T("CPUSHL"), NULL },
	{ i_CPUSHP, _T("CPUSHP"), NULL },
	{ i_CPUSHA, _T("CPUSHA"), NULL },
	{ i_MOVE16, _T("MOVE16"), NULL },

	{ i_MMUOP030, _T("MMUOP030"), NULL },
	{ i_PFLUSHN, _T("PFLUSHN"), NULL },
	{ i_PFLUSH, _T("PFLUSH"), NULL },
	{ i_PFLUSHAN, _T("PFLUSHAN"), NULL },
	{ i_PFLUSHA, _T("PFLUSHA"), NULL },

	{ i_PLPAR, _T("PLPAR"), NULL },
	{ i_PLPAW, _T("PLPAW"), NULL },
	{ i_PTESTR, _T("PTESTR"), NULL },
	{ i_PTESTW, _T("PTESTW"), NULL },

	{ i_LPSTOP, _T("LPSTOP"), NULL },
	{ i_ILLG, _T(""), NULL },
};

struct instr *table68k;

static amodes mode_from_str (const char *str)
{
	if (strncmp (str, _T("Dreg"), 4) == 0) return Dreg;
	if (strncmp (str, _T("Areg"), 4) == 0) return Areg;
	if (strncmp (str, _T("Aind"), 4) == 0) return Aind;
	if (strncmp (str, _T("Apdi"), 4) == 0) return Apdi;
	if (strncmp (str, _T("Aipi"), 4) == 0) return Aipi;
	if (strncmp (str, _T("Ad16"), 4) == 0) return Ad16;
	if (strncmp (str, _T("Ad8r"), 4) == 0) return Ad8r;
	if (strncmp (str, _T("absw"), 4) == 0) return absw;
	if (strncmp (str, _T("absl"), 4) == 0) return absl;
	if (strncmp (str, _T("PC16"), 4) == 0) return PC16;
	if (strncmp (str, _T("PC8r"), 4) == 0) return PC8r;
	if (strncmp (str, _T("Immd"), 4) == 0) return imm;
	abort ();
	return 0;
}

STATIC_INLINE amodes mode_from_mr (int mode, int reg)
{
	switch (mode) {
	case 0: return Dreg;
	case 1: return Areg;
	case 2: return Aind;
	case 3: return Aipi;
	case 4: return Apdi;
	case 5: return Ad16;
	case 6: return Ad8r;
	case 7:
		switch (reg) {
		case 0: return absw;
		case 1: return absl;
		case 2: return PC16;
		case 3: return PC8r;
		case 4: return imm;
		case 5:
		case 6:
		case 7: return am_illg;
		}
	}
	abort ();
	return 0;
}

static void build_insn (int insn)
{
	int find = -1;
	int variants;
	int isjmp = 0;
	struct instr_def id;
	const char *opcstr;
	int i;

	int flaglive = 0, flagdead = 0;

	id = defs68k[insn];

	/* Note: We treat anything with unknown flags as a jump. That
	is overkill, but "the programmer" was lazy quite often, and
	*this* programmer can't be bothered to work out what can and
	can't trap. Usually, this will be overwritten with the gencomp
	based information, anyway. */

	for (i = 0; i < 5; i++) {
		switch (id.flaginfo[i].flagset){
		case fa_unset: break;
		case fa_isjmp: isjmp = 1; break;
		case fa_isbranch: isjmp = 1; break;
		case fa_zero: flagdead |= 1 << i; break;
		case fa_one: flagdead |= 1 << i; break;
		case fa_dontcare: flagdead |= 1 << i; break;
		case fa_unknown: isjmp = 1; flagdead = -1; goto out1;
		case fa_set: flagdead |= 1 << i; break;
		}
	}

out1:
	for (i = 0; i < 5; i++) {
		switch (id.flaginfo[i].flaguse) {
		case fu_unused: break;
		case fu_isjmp: isjmp = 1; flaglive |= 1 << i; break;
		case fu_maybecc: isjmp = 1; flaglive |= 1 << i; break;
		case fu_unknown: isjmp = 1; flaglive |= 1 << i; break;
		case fu_used: flaglive |= 1 << i; break;
		}
	}

	opcstr = id.opcstr;
	for (variants = 0; variants < (1 << id.n_variable); variants++) {
		int bitcnt[lastbit];
		int bitval[lastbit];
		int bitpos[lastbit];
		int i;
		uae_u16 opc = id.bits;
		uae_u16 msk, vmsk;
		int pos = 0;
		int mnp = 0;
		int bitno = 0;
		char mnemonic[10];

		wordsizes sz = sz_long;
		int srcgather = 0, dstgather = 0;
		int usesrc = 0, usedst = 0;
		int srctype = 0;
		int srcpos = -1, dstpos = -1;

		amodes srcmode = am_unknown, destmode = am_unknown;
		int srcreg = -1, destreg = -1;

		for (i = 0; i < lastbit; i++)
			bitcnt[i] = bitval[i] = 0;

		vmsk = 1 << id.n_variable;

		for (i = 0, msk = 0x8000; i < 16; i++, msk >>= 1) {
			if (!(msk & id.mask)) {
				int currbit = id.bitpos[bitno++];
				int bit_set;
				vmsk >>= 1;
				bit_set = variants & vmsk ? 1 : 0;
				if (bit_set)
					opc |= msk;
				bitpos[currbit] = 15 - i;
				bitcnt[currbit]++;
				bitval[currbit] <<= 1;
				bitval[currbit] |= bit_set;
			}
		}

		if (bitval[bitj] == 0) bitval[bitj] = 8;
		/* first check whether this one does not match after all */
		if (bitval[bitz] == 3 || bitval[bitC] == 1)
			continue;
		if (bitcnt[bitI] && (bitval[bitI] == 0x00 || bitval[bitI] == 0xff))
			continue;

		/* bitI and bitC get copied to biti and bitc */
		if (bitcnt[bitI]) {
			bitval[biti] = bitval[bitI]; bitpos[biti] = bitpos[bitI];
		}
		if (bitcnt[bitC])
			bitval[bitc] = bitval[bitC];

		pos = 0;
		while (opcstr[pos] && !isspace(opcstr[pos])) {
			if (opcstr[pos] == '.') {
				pos++;
				switch (opcstr[pos]) {

				case 'B': sz = sz_byte; break;
				case 'W': sz = sz_word; break;
				case 'L': sz = sz_long; break;
				case 'z':
					switch (bitval[bitz]) {
					case 0: sz = sz_byte; break;
					case 1: sz = sz_word; break;
					case 2: sz = sz_long; break;
					default: abort();
					}
					break;
				default: abort();
				}
			} else {
				mnemonic[mnp] = opcstr[pos];
				if (mnemonic[mnp] == 'f') {
					find = -1;
					switch (bitval[bitf]) {
					case 0: mnemonic[mnp] = 'R'; break;
					case 1: mnemonic[mnp] = 'L'; break;
					default: abort();
					}
				}
				mnp++;
			}
			pos++;
		}
		mnemonic[mnp] = 0;

		/* now, we have read the mnemonic and the size */
		while (opcstr[pos] && isspace(opcstr[pos]))
			pos++;

		/* A goto a day keeps the D******a away. */
		if (opcstr[pos] == 0)
			goto endofline;

		/* parse the source address */
		usesrc = 1;
		switch (opcstr[pos++]) {
		case 'D':
			srcmode = Dreg;
			switch (opcstr[pos++]) {
			case 'r': srcreg = bitval[bitr]; srcgather = 1; srcpos = bitpos[bitr]; break;
			case 'R': srcreg = bitval[bitR]; srcgather = 1; srcpos = bitpos[bitR]; break;
			default: abort();
			}
			break;
		case 'A':
			srcmode = Areg;
			switch (opcstr[pos++]) {
			case 'r': srcreg = bitval[bitr]; srcgather = 1; srcpos = bitpos[bitr]; break;
			case 'R': srcreg = bitval[bitR]; srcgather = 1; srcpos = bitpos[bitR]; break;
			default: abort();
			}
			switch (opcstr[pos]) {
			case 'p': srcmode = Apdi; pos++; break;
			case 'P': srcmode = Aipi; pos++; break;
			case 'a': srcmode = Aind; pos++; break;
			}
			break;
		case 'L':
			srcmode = absl;
			break;
		case '#':
			switch (opcstr[pos++]) {
			case 'z': srcmode = imm; break;
			case '0': srcmode = imm0; break;
			case '1': srcmode = imm1; break;
			case '2': srcmode = imm2; break;
			case 'i': srcmode = immi; srcreg = (uae_s32)(uae_s8)bitval[biti];
				if (CPU_EMU_SIZE < 4) {
					/* Used for branch instructions */
					srctype = 1;
					srcgather = 1;
					srcpos = bitpos[biti];
				}
				break;
			case 'j': srcmode = immi; srcreg = bitval[bitj];
				if (CPU_EMU_SIZE < 3) {
					/* 1..8 for ADDQ/SUBQ and rotshi insns */
					srcgather = 1;
					srctype = 3;
					srcpos = bitpos[bitj];
				}
				break;
			case 'J': srcmode = immi; srcreg = bitval[bitJ];
				if (CPU_EMU_SIZE < 5) {
					/* 0..15 */
					srcgather = 1;
					srctype = 2;
					srcpos = bitpos[bitJ];
				}
				break;
			case 'k': srcmode = immi; srcreg = bitval[bitk];
				if (CPU_EMU_SIZE < 3) {
					srcgather = 1;
					srctype = 4;
					srcpos = bitpos[bitk];
				}
				break;
			case 'K': srcmode = immi; srcreg = bitval[bitK];
				if (CPU_EMU_SIZE < 5) {
					/* 0..15 */
					srcgather = 1;
					srctype = 5;
					srcpos = bitpos[bitK];
				}
				break;
			case 'p': srcmode = immi; srcreg = bitval[bitK];
				if (CPU_EMU_SIZE < 5) {
					/* 0..3 */
					srcgather = 1;
					srctype = 7;
					srcpos = bitpos[bitp];
				}
				break;
			default: abort();
			}
			break;
		case 'd':
			srcreg = bitval[bitD];
			srcmode = mode_from_mr(bitval[bitd],bitval[bitD]);
			if (srcmode == am_illg)
				continue;
			if (CPU_EMU_SIZE < 2 &&
				(srcmode == Areg || srcmode == Dreg || srcmode == Aind
				|| srcmode == Ad16 || srcmode == Ad8r || srcmode == Aipi
				|| srcmode == Apdi))
			{
				srcgather = 1; srcpos = bitpos[bitD];
			}
			if (opcstr[pos] == '[') {
				pos++;
				if (opcstr[pos] == '!') {
					/* exclusion */
					do {
						pos++;
						if (mode_from_str(opcstr+pos) == srcmode)
							goto nomatch;
						pos += 4;
					} while (opcstr[pos] == ',');
					pos++;
				} else {
					if (opcstr[pos+4] == '-') {
						/* replacement */
						if (mode_from_str(opcstr+pos) == srcmode)
							srcmode = mode_from_str(opcstr+pos+5);
						else
							goto nomatch;
						pos += 10;
					} else {
						/* normal */
						while(mode_from_str(opcstr+pos) != srcmode) {
							pos += 4;
							if (opcstr[pos] == ']')
								goto nomatch;
							pos++;
						}
						while(opcstr[pos] != ']') pos++;
						pos++;
						break;
					}
				}
			}
			/* Some addressing modes are invalid as destination */
			if (srcmode == imm || srcmode == PC16 || srcmode == PC8r)
				goto nomatch;
			break;
		case 's':
			srcreg = bitval[bitS];
			srcmode = mode_from_mr(bitval[bits],bitval[bitS]);

			if (srcmode == am_illg)
				continue;
			if (CPU_EMU_SIZE < 2 &&
				(srcmode == Areg || srcmode == Dreg || srcmode == Aind
				|| srcmode == Ad16 || srcmode == Ad8r || srcmode == Aipi
				|| srcmode == Apdi))
			{
				srcgather = 1; srcpos = bitpos[bitS];
			}
			if (opcstr[pos] == '[') {
				pos++;
				if (opcstr[pos] == '!') {
					/* exclusion */
					do {
						pos++;
						if (mode_from_str(opcstr+pos) == srcmode)
							goto nomatch;
						pos += 4;
					} while (opcstr[pos] == ',');
					pos++;
				} else {
					if (opcstr[pos+4] == '-') {
						/* replacement */
						if (mode_from_str(opcstr+pos) == srcmode)
							srcmode = mode_from_str(opcstr+pos+5);
						else
							goto nomatch;
						pos += 10;
					} else {
						/* normal */
						while(mode_from_str(opcstr+pos) != srcmode) {
							pos += 4;
							if (opcstr[pos] == ']')
								goto nomatch;
							pos++;
						}
						while(opcstr[pos] != ']') pos++;
						pos++;
					}
				}
			}
			break;
		default: abort();
		}
		/* safety check - might have changed */
		if (srcmode != Areg && srcmode != Dreg && srcmode != Aind
			&& srcmode != Ad16 && srcmode != Ad8r && srcmode != Aipi
			&& srcmode != Apdi && srcmode != immi)
		{
			srcgather = 0;
		}
		if (srcmode == Areg && sz == sz_byte)
			goto nomatch;

		if (opcstr[pos] != ',')
			goto endofline;
		pos++;

		/* parse the destination address */
		usedst = 1;
		switch (opcstr[pos++]) {
		case 'D':
			destmode = Dreg;
			switch (opcstr[pos++]) {
			case 'r': destreg = bitval[bitr]; dstgather = 1; dstpos = bitpos[bitr]; break;
			case 'R': destreg = bitval[bitR]; dstgather = 1; dstpos = bitpos[bitR]; break;
			default: abort();
			}
			if (dstpos < 0 || dstpos >= 32)
				abort ();
			break;
		case 'A':
			destmode = Areg;
			switch (opcstr[pos++]) {
			case 'r': destreg = bitval[bitr]; dstgather = 1; dstpos = bitpos[bitr]; break;
			case 'R': destreg = bitval[bitR]; dstgather = 1; dstpos = bitpos[bitR]; break;
			case 'x': destreg = 0; dstgather = 0; dstpos = 0; break;
			default: abort();
			}
			if (dstpos < 0 || dstpos >= 32)
				abort ();
			switch (opcstr[pos]) {
			case 'p': destmode = Apdi; pos++; break;
			case 'P': destmode = Aipi; pos++; break;
			}
			break;
		case 'L':
			destmode = absl;
			break;
		case '#':
			switch (opcstr[pos++]) {
			case 'z': destmode = imm; break;
			case '0': destmode = imm0; break;
			case '1': destmode = imm1; break;
			case '2': destmode = imm2; break;
			case 'i': destmode = immi; destreg = (uae_s32)(uae_s8)bitval[biti]; break;
			case 'j': destmode = immi; destreg = bitval[bitj]; break;
			case 'J': destmode = immi; destreg = bitval[bitJ]; break;
			case 'k': destmode = immi; destreg = bitval[bitk]; break;
			case 'K': destmode = immi; destreg = bitval[bitK]; break;
			default: abort();
			}
			break;
		case 'd':
			destreg = bitval[bitD];
			destmode = mode_from_mr(bitval[bitd],bitval[bitD]);
			if (destmode == am_illg)
				continue;
			if (CPU_EMU_SIZE < 1 &&
				(destmode == Areg || destmode == Dreg || destmode == Aind
				|| destmode == Ad16 || destmode == Ad8r || destmode == Aipi
				|| destmode == Apdi))
			{
				dstgather = 1; dstpos = bitpos[bitD];
			}

			if (opcstr[pos] == '[') {
				pos++;
				if (opcstr[pos] == '!') {
					/* exclusion */
					do {
						pos++;
						if (mode_from_str(opcstr+pos) == destmode)
							goto nomatch;
						pos += 4;
					} while (opcstr[pos] == ',');
					pos++;
				} else {
					if (opcstr[pos+4] == '-') {
						/* replacement */
						if (mode_from_str(opcstr+pos) == destmode)
							destmode = mode_from_str(opcstr+pos+5);
						else
							goto nomatch;
						pos += 10;
					} else {
						/* normal */
						while(mode_from_str(opcstr+pos) != destmode) {
							pos += 4;
							if (opcstr[pos] == ']')
								goto nomatch;
							pos++;
						}
						while(opcstr[pos] != ']') pos++;
						pos++;
						break;
					}
				}
			}
			/* Some addressing modes are invalid as destination */
			if (destmode == imm || destmode == PC16 || destmode == PC8r)
				goto nomatch;
			break;
		case 's':
			destreg = bitval[bitS];
			destmode = mode_from_mr(bitval[bits],bitval[bitS]);

			if (destmode == am_illg)
				continue;
			if (CPU_EMU_SIZE < 1 &&
				(destmode == Areg || destmode == Dreg || destmode == Aind
				|| destmode == Ad16 || destmode == Ad8r || destmode == Aipi
				|| destmode == Apdi))
			{
				dstgather = 1; dstpos = bitpos[bitS];
			}

			if (opcstr[pos] == '[') {
				pos++;
				if (opcstr[pos] == '!') {
					/* exclusion */
					do {
						pos++;
						if (mode_from_str(opcstr+pos) == destmode)
							goto nomatch;
						pos += 4;
					} while (opcstr[pos] == ',');
					pos++;
				} else {
					if (opcstr[pos+4] == '-') {
						/* replacement */
						if (mode_from_str(opcstr+pos) == destmode)
							destmode = mode_from_str(opcstr+pos+5);
						else
							goto nomatch;
						pos += 10;
					} else {
						/* normal */
						while(mode_from_str(opcstr+pos) != destmode) {
							pos += 4;
							if (opcstr[pos] == ']')
								goto nomatch;
							pos++;
						}
						while(opcstr[pos] != ']') pos++;
						pos++;
					}
				}
			}
			break;
		default: abort();
		}
		/* safety check - might have changed */
		if (destmode != Areg && destmode != Dreg && destmode != Aind
			&& destmode != Ad16 && destmode != Ad8r && destmode != Aipi
			&& destmode != Apdi)
		{
			dstgather = 0;
		}

		if (destmode == Areg && sz == sz_byte)
			goto nomatch;
#if 0
		if (sz == sz_byte && (destmode == Aipi || destmode == Apdi)) {
			dstgather = 0;
		}
#endif
endofline:
		/* now, we have a match */
		/* if (table68k[opc].mnemo != i_ILLG)
			write_log (_T("Double match: %x: %s\n"), opc, opcstr); */
		if (find == -1) {
			for (find = 0;; find++) {
				if (strcmp (mnemonic, lookuptab[find].name) == 0) {
					table68k[opc].mnemo = lookuptab[find].mnemo;
					break;
				}
				if (strlen (lookuptab[find].name) == 0)
					abort();
			}
		}
		else {
			table68k[opc].mnemo = lookuptab[find].mnemo;
		}
		table68k[opc].cc = bitval[bitc];
		if (table68k[opc].mnemo == i_BTST
			|| table68k[opc].mnemo == i_BSET
			|| table68k[opc].mnemo == i_BCLR
			|| table68k[opc].mnemo == i_BCHG)
		{
			sz = destmode == Dreg ? sz_long : sz_byte;
		}
		table68k[opc].size = sz;
		table68k[opc].sreg = srcreg;
		table68k[opc].dreg = destreg;
		table68k[opc].smode = srcmode;
		table68k[opc].dmode = destmode;
		table68k[opc].spos = srcgather ? srcpos : -1;
		table68k[opc].dpos = dstgather ? dstpos : -1;
		table68k[opc].suse = usesrc;
		table68k[opc].duse = usedst;
		table68k[opc].stype = srctype;
		table68k[opc].plev = id.plevel;
		table68k[opc].clev = id.cpulevel;
		table68k[opc].unimpclev = id.unimpcpulevel;

#if 0
		for (i = 0; i < 5; i++) {
			table68k[opc].flaginfo[i].flagset = id.flaginfo[i].flagset;
			table68k[opc].flaginfo[i].flaguse = id.flaginfo[i].flaguse;
		}
#endif
		table68k[opc].flagdead = flagdead;
		table68k[opc].flaglive = flaglive;
		table68k[opc].isjmp = isjmp;
nomatch:
		/* FOO! */;
	}
}


void read_table68k (void)
{
	int i;

	xfree (table68k);
	table68k = xmalloc (struct instr, 65536);
	for (i = 0; i < 65536; i++) {
		table68k[i].mnemo = i_ILLG;
		table68k[i].handler = -1;
	}
	for (i = 0; i < n_defs68k; i++) {
		build_insn (i);
	}
}

static int imismatch;

static void handle_merges (long int opcode)
{
	uae_u16 smsk;
	uae_u16 dmsk;
	int sbitdst, dstend;
	int srcreg, dstreg;

	if (table68k[opcode].spos == -1) {
		sbitdst = 1; smsk = 0;
	} else {
		switch (table68k[opcode].stype) {
		case 0:
			smsk = 7; sbitdst = 8; break;
		case 1:
			smsk = 255; sbitdst = 256; break;
		case 2:
			smsk = 15; sbitdst = 16; break;
		case 3:
			smsk = 7; sbitdst = 8; break;
		case 4:
			smsk = 7; sbitdst = 8; break;
		case 5:
			smsk = 63; sbitdst = 64; break;
		case 7:
			smsk = 3; sbitdst = 4; break;
		default:
			smsk = 0; sbitdst = 0;
			abort();
			break;
		}
		smsk <<= table68k[opcode].spos;
	}
	if (table68k[opcode].dpos == -1) {
		dstend = 1; dmsk = 0;
	} else {
		dmsk = 7 << table68k[opcode].dpos;
		dstend = 8;
	}
	for (srcreg=0; srcreg < sbitdst; srcreg++) {
		for (dstreg=0; dstreg < dstend; dstreg++) {
			uae_u16 code = (uae_u16)opcode;

			code = (code & ~smsk) | (srcreg << table68k[opcode].spos);
			code = (code & ~dmsk) | (dstreg << table68k[opcode].dpos);

			/* Check whether this is in fact the same instruction.
			* The instructions should never differ, except for the
			* Bcc.(BW) case. */
			if (table68k[code].mnemo != table68k[opcode].mnemo
				|| table68k[code].size != table68k[opcode].size
				|| table68k[code].suse != table68k[opcode].suse
				|| table68k[code].duse != table68k[opcode].duse)
			{
				imismatch++; continue;
			}
			if (table68k[opcode].suse
				&& (table68k[opcode].spos != table68k[code].spos
				|| table68k[opcode].smode != table68k[code].smode
				|| table68k[opcode].stype != table68k[code].stype))
			{
				imismatch++; continue;
			}
			if (table68k[opcode].duse
				&& (table68k[opcode].dpos != table68k[code].dpos
				|| table68k[opcode].dmode != table68k[code].dmode))
			{
				imismatch++; continue;
			}

			if (code != opcode)
				table68k[code].handler = opcode;
		}
	}
}

void do_merges (void)
{
	long int opcode;
	int nr = 0;
	imismatch = 0;
	for (opcode = 0; opcode < 65536; opcode++) {
		if (table68k[opcode].handler != -1 || table68k[opcode].mnemo == i_ILLG)
			continue;
		nr++;
		handle_merges (opcode);
	}
	nr_cpuop_funcs = nr;
}

int get_no_mismatches (void)
{
	return imismatch;
}
