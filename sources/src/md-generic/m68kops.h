/*
 * E-UAE - The portable Amiga Emulator
 *
 * MC68000 emulation - machine-dependent optimized operations
 *
 * (c) 2004-2007 Richard Drummond
 */

#ifndef EUAE_MACHDEP_M68KOPS_H
#define EUAE_MACHDEP_M68KOPS_H

/*
 * Test operations
 *
 * Evaluate operand and set Z and N flags. Always clear C and V.
 */

#define optflag_testl(v) \
    do { \
		CLEAR_CZNV ();\
		SET_ZFLG   ( (((uae_s32)(v)) == 0) ? 1 : 0);\
		SET_NFLG   ( (((uae_s32)(v)) < 0) ? 1 : 0); \
    } while (0)

#define optflag_testw(v) \
    do { \
		CLEAR_CZNV ();\
		SET_ZFLG   ( (((uae_s16)(v)) == 0) ? 1 : 0);\
		SET_NFLG   ( (((uae_s16)(v)) < 0) ? 1 : 0); \
    } while (0)

#define optflag_testb(v) \
    do { \
		CLEAR_CZNV ();\
		SET_ZFLG   ( (((uae_s8)(v)) == 0) ? 1 : 0);\
		SET_NFLG   ( (((uae_s8)(v)) < 0) ? 1 : 0); \
    } while (0)



/*
 * Add operations
 *
 * Perform v = s + d and set ZNCV accordingly
 */

#define optflag_addl(v, s, d) \
 do {\
	v = ((uae_s32)(d)) + ((uae_s32)(s)); \
	int flgs = (((uae_s32)(s)) < 0) ? 1 : 0; \
	int flgo = (((uae_s32)(d)) < 0) ? 1 : 0; \
	int flgn = (((uae_s32)(v)) < 0) ? 1 : 0; \
	SET_ZFLG ( (((uae_s32)(v)) == 0) ? 1 : 0); \
	SET_VFLG ( (flgs ^ flgn) & (flgo ^ flgn)); \
	SET_CFLG ( (((uae_u32)(~d)) < ((uae_u32)(s))) ? 1 : 0); \
	COPY_CARRY (); \
	SET_NFLG ( flgn); \
} while (0)

#define optflag_addw(v, s, d) \
 do {\
	v = ((uae_s16)(d)) + ((uae_s16)(s)); \
	int flgs = (((uae_s16)(s)) < 0) ? 1 : 0; \
	int flgo = (((uae_s16)(d)) < 0) ? 1 : 0; \
	int flgn = (((uae_s16)(v)) < 0) ? 1 : 0; \
	SET_ZFLG ( (((uae_s16)(v)) == 0) ? 1 : 0); \
	SET_VFLG ( (flgs ^ flgn) & (flgo ^ flgn)); \
	SET_CFLG ( (((uae_u16)(~d)) < ((uae_u16)(s))) ? 1 : 0); \
	COPY_CARRY (); \
	SET_NFLG ( flgn); \
} while (0)

#define optflag_addb(v, s, d) \
 do {\
	v = ((uae_s8)(d)) + ((uae_s8)(s)); \
	int flgs = (((uae_s8)(s)) < 0) ? 1 : 0; \
	int flgo = (((uae_s8)(d)) < 0) ? 1 : 0; \
	int flgn = (((uae_s8)(v)) < 0) ? 1 : 0; \
	SET_ZFLG ( (((uae_s8)(v)) == 0) ? 1 : 0); \
	SET_VFLG ( (flgs ^ flgn) & (flgo ^ flgn)); \
	SET_CFLG ( (((uae_u8)(~d)) < ((uae_u8)(s))) ? 1 : 0); \
	COPY_CARRY (); \
	SET_NFLG ( flgn); \
} while (0)

/*
 * Subtraction operations
 *
 * Perform v = d - s and set ZNCV accordingly
 */

#define optflag_subb(v, s, d) \
do {\
	v = ((uae_s8)(d)) - ((uae_s8)(s)); \
	int flgs = (((uae_s8)(s)) < 0) ? 1 : 0; \
	int flgo = (((uae_s8)(d)) < 0) ? 1 : 0; \
	int flgn = (((uae_s8)(v)) < 0) ? 1 : 0; \
	SET_ZFLG ( (((uae_s8)(v)) == 0) ? 1 : 0); \
	SET_VFLG ( (flgs ^ flgo) & (flgn ^ flgo)); \
	SET_CFLG ( (((uae_u8)(s)) > ((uae_u8)(d))) ? 1 : 0); \
	COPY_CARRY (); \
	SET_NFLG ( flgn); \
} while (0)

#define optflag_subw(v, s, d) \
do {\
	v = ((uae_s16)(d)) - ((uae_s16)(s)); \
	int flgs = (((uae_s16)(s)) < 0) ? 1 : 0; \
	int flgo = (((uae_s16)(d)) < 0) ? 1 : 0; \
	int flgn = (((uae_s16)(v)) < 0) ? 1 : 0; \
	SET_ZFLG ( (((uae_s16)(v)) == 0) ? 1 : 0); \
	SET_VFLG ( (flgs ^ flgo) & (flgn ^ flgo)); \
	SET_CFLG ( (((uae_u16)(s)) > ((uae_u16)(d))) ? 1 : 0); \
	COPY_CARRY (); \
	SET_NFLG ( flgn); \
} while (0)

#define optflag_subl(v, s, d) \
do {\
	v = ((uae_s32)(d)) - ((uae_s32)(s)); \
	int flgs = (((uae_s32)(s)) < 0) ? 1 : 0; \
	int flgo = (((uae_s32)(d)) < 0) ? 1 : 0; \
	int flgn = (((uae_s32)(v)) < 0) ? 1 : 0; \
	SET_ZFLG ( (((uae_s32)(v)) == 0) ? 1 : 0); \
	SET_VFLG ( (flgs ^ flgo) & (flgn ^ flgo)); \
	SET_CFLG ( (((uae_u32)(s)) > ((uae_u32)(d))) ? 1 : 0); \
	COPY_CARRY (); \
	SET_NFLG ( flgn); \
} while (0)


/*
 * Compare operations
 */

#define optflag_cmpb(s, d) \
do {\
uae_u32 	v = ((uae_s8)(d)) - ((uae_s8)(s)); \
	int flgs = (((uae_s8)(s)) < 0) ? 1 : 0; \
	int flgo = (((uae_s8)(d)) < 0) ? 1 : 0; \
	int flgn = (((uae_s8)(v)) < 0) ? 1 : 0; \
	SET_ZFLG ( (((uae_s8)(v)) == 0) ? 1 : 0); \
	SET_VFLG ( (flgs != flgo) && (flgn != flgo));\
	SET_CFLG ( (((uae_u8)(s)) > ((uae_u8)(d))) ? 1 : 0); \
	SET_NFLG ( flgn); \
}while (0)


#define optflag_cmpw(s, d) \
do {\
uae_u32 	v = ((uae_s16)(d)) - ((uae_s16)(s)); \
	int flgs = (((uae_s16)(s)) < 0) ? 1 : 0; \
	int flgo = (((uae_s16)(d)) < 0) ? 1 : 0; \
	int flgn = (((uae_s16)(v)) < 0) ? 1 : 0; \
	SET_ZFLG ( (((uae_s16)(v)) == 0) ? 1 : 0); \
	SET_VFLG ( (flgs != flgo) && (flgn != flgo));\
	SET_CFLG ( (((uae_u16)(s)) > ((uae_u16)(d))) ? 1 : 0); \
	SET_NFLG ( flgn); \
}while (0)

#define optflag_cmpl(s, d) \
do {\
uae_u32 	v = ((uae_s32)(d)) - ((uae_s32)(s)); \
	int flgs = (((uae_s32)(s)) < 0) ? 1 : 0; \
	int flgo = (((uae_s32)(d)) < 0) ? 1 : 0; \
	int flgn = (((uae_s32)(v)) < 0) ? 1 : 0; \
	SET_ZFLG ( (((uae_s32)(v)) == 0) ? 1 : 0); \
	SET_VFLG ( (flgs != flgo) && (flgn != flgo));\
	SET_CFLG ( (((uae_u32)(s)) > ((uae_u32)(d))) ? 1 : 0); \
	SET_NFLG ( flgn); \
}while (0)

#endif /* EUAE_MACHDEP_M68KOPS_H */
