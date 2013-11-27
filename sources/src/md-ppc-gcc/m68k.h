 /*
  * UAE - The Un*x Amiga Emulator
  *
  * MC68000 emulation - machine dependent bits
  *
  * Copyright 1996 Bernd Schmidt
  * Copyright 2004-2007 Richard Drummond
  */

 /*
  * Machine dependent structure for holding the 68k CCR flags
  */
struct flag_struct {
    unsigned int cznv;
    unsigned int x;
};

extern struct flag_struct regflags;

/*
 * The bits in the cznv field in the above structure are assigned to
 * allow the easy mirroring of the PPC condition flags (the V and C are
 * assigned so that they can be easily copied from the XER register with
 * a 'mcrxr cr2').
 *
 * The 68k CZNV flags are thus assigned in the cznv field as:
 *
 *      <cr0> <cr1> <cr2> <cr3> <cr4> <cr5> <cr6> <cr7>
 * bit:  00    04    08    0C    10    14    18    1C
 *       |     |     |     |     |     |     |     |
 * flag: N-Z-  ----  -VC-  ----  ----  ----  ----  ----
 *
 * The X flag in the flag_struct x field is assigned to make it easy to
 * copy from the C flag:
 *
 * bit:  00    04    08    0C    10    14    18    1C
 *       |     |     |     |     |     |     |     |
 * flag: ----  ----  --X-  ----  ----  ----  ----  ----
 *
 * Note: The PPC convention is that the MSB is bit 0. Don't get confused.
 *
 * Note: The PPC Carry flag has the the opposite sense of the 68k
 * Carry flag following a substractions. Thus, after a substraction, the
 * C bit in the above needs to be flipped.
 */

#define FLAGBIT_N	31
#define FLAGBIT_Z	29
#define FLAGBIT_V	22
#define FLAGBIT_C	21
#define FLAGBIT_X	21

#define FLAGVAL_N	(1 << FLAGBIT_N)
#define FLAGVAL_Z 	(1 << FLAGBIT_Z)
#define FLAGVAL_C	(1 << FLAGBIT_C)
#define FLAGVAL_V	(1 << FLAGBIT_V)
#define FLAGVAL_X	(1 << FLAGBIT_X)

#define SET_ZFLG(y)	(regflags.cznv = (regflags.cznv & ~FLAGVAL_Z) | (((y) ? 1 : 0) << FLAGBIT_Z))
#define SET_CFLG(y)	(regflags.cznv = (regflags.cznv & ~FLAGVAL_C) | (((y) ? 1 : 0) << FLAGBIT_C))
#define SET_VFLG(y)	(regflags.cznv = (regflags.cznv & ~FLAGVAL_V) | (((y) ? 1 : 0) << FLAGBIT_V))
#define SET_NFLG(y)	(regflags.cznv = (regflags.cznv & ~FLAGVAL_N) | (((y) ? 1 : 0) << FLAGBIT_N))
#define SET_XFLG(y)	(regflags.x    = ((y) ? 1 : 0) << FLAGBIT_X)

#define GET_ZFLG()	((regflags.cznv >> FLAGBIT_Z) & 1)
#define GET_CFLG()	((regflags.cznv >> FLAGBIT_C) & 1)
#define GET_VFLG()	((regflags.cznv >> FLAGBIT_V) & 1)
#define GET_NFLG()	((regflags.cznv >> FLAGBIT_N) & 1)
#define GET_XFLG()	((regflags.x    >> FLAGBIT_X) & 1)

#define CLEAR_CZNV()	(regflags.cznv  = 0)
#define GET_CZNV	(regflags.cznv)
#define IOR_CZNV(X) (regflags.cznv |= (X))
#define SET_CZNV(X) (regflags.cznv = (X))

#define COPY_CARRY() (regflags.x = regflags.cznv)


/*
 * Test CCR condition
 */
STATIC_INLINE int cctrue(int cc)
{
    uae_u32 cznv = regflags.cznv;

    switch (cc) {
	case 0:  return 1;								/*				T  */
	case 1:  return 0;								/*				F  */
	case 2:  return (cznv & (FLAGVAL_C | FLAGVAL_Z)) == 0;				/* !CFLG && !ZFLG		HI */
	case 3:  return (cznv & (FLAGVAL_C | FLAGVAL_Z)) != 0;				/*  CFLG || ZFLG		LS */
	case 4:  return (cznv & FLAGVAL_C) == 0;					/* !CFLG			CC */
	case 5:  return (cznv & FLAGVAL_C) != 0;					/*  CFLG			CS */
	case 6:  return (cznv & FLAGVAL_Z) == 0;					/* !ZFLG			NE */
	case 7:  return (cznv & FLAGVAL_Z) != 0;					/*  ZFLG			EQ */
	case 8:  return (cznv & FLAGVAL_V) == 0;					/* !VFLG			VC */
	case 9:  return (cznv & FLAGVAL_V) != 0;					/*  VFLG			VS */
	case 10: return (cznv & FLAGVAL_N) == 0;					/* !NFLG			PL */
	case 11: return (cznv & FLAGVAL_N) != 0;					/*  NFLG			MI */
	case 12: return (((cznv << (FLAGBIT_N - FLAGBIT_V)) ^ cznv) & FLAGVAL_N) == 0;	/*  NFLG == VFLG		GE */
	case 13: return (((cznv << (FLAGBIT_N - FLAGBIT_V)) ^ cznv) & FLAGVAL_N) != 0;	/*  NFLG != VFLG		LT */
	case 14: cznv &= (FLAGVAL_N | FLAGVAL_Z | FLAGVAL_V);				/* ZFLG && (NFLG == VFLG)	GT */
		 return (((cznv << (FLAGBIT_N - FLAGBIT_V)) ^ cznv) & (FLAGVAL_N | FLAGVAL_Z)) == 0;
	case 15: cznv &= (FLAGVAL_N | FLAGVAL_Z | FLAGVAL_V);				/* ZFLG && (NFLG != VFLG)	LE */
		 return (((cznv << (FLAGBIT_N - FLAGBIT_V)) ^ cznv) & (FLAGVAL_N | FLAGVAL_Z)) != 0;
    }
    abort ();
    return 0;
}
