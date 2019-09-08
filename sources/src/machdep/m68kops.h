/*
 * E-UAE - The portable Amiga Emulator
 *
 * MC68000 emulation - machine-dependent optimized operations
 *
 * (c) 2004-2007 Richard Drummond
 */

#ifndef EUAE_MACHDEP_M68KOPS_H
#define EUAE_MACHDEP_M68KOPS_H

#ifndef ANDROID

#if defined(__CELLOS_LV2__) || defined(_WIN32) || defined(__x86_64__) || defined(ARM) || defined(WIIU)

#ifdef WIIU
#define FLAGBIT_N	31
#define FLAGBIT_Z	29
#define FLAGBIT_V	22
#define FLAGBIT_C	21
#define FLAGBIT_X	21
#else
#define FLAGBIT_N	15
#define FLAGBIT_Z	14
#define FLAGBIT_C	8
#define FLAGBIT_V	0
#define FLAGBIT_X	8
#endif

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
#define IOR_CZNV(X)	(regflags.cznv |= (X))
#define SET_CZNV(X)	(regflags.cznv  = (X))

#define COPY_CARRY() (regflags.x = regflags.cznv)

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

#else

#warning "OPT TESTFLAG"
/*
 * Optimized code which uses the host CPU's condition flags to evaluate
 * 68K CCR flags for certain operations.
 *
 * These are used by various opcode handlers when
 * gencpu has been built with OPTIMIZED_FLAGS defined
 */

/*
 * Test operations
 *
 * Evaluate operand and set Z and N flags. Always clear C and V.
 */
/* Is there any way to do this without declaring *all* memory clobbered?
   I.e. any way to tell gcc that some byte-sized value is in %al? */
#ifdef __APPLE__
#define optflag_testl(v) \
  __asm__ __volatile__ ("andl %0,%0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,_regflags\n\t" \
			"movb %%ah,_regflags+1\n\t" \
			:: "r" (v) : "%eax","cc","memory")
#define optflag_testw(v) \
  __asm__ __volatile__ ("andw %w0,%w0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,_regflags\n\t" \
			"movb %%ah,_regflags+1\n\t" \
			:: "r" (v) : "%eax","cc","memory")

#define optflag_testb(v) \
  __asm__ __volatile__ ("andb %b0,%b0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,_regflags\n\t" \
			"movb %%ah,_regflags+1\n\t" \
			:: "q" (v) : "%eax","cc","memory")

#define optflag_addl(v, s, d) do { \
  __asm__ __volatile__ ("addl %k1,%k0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,_regflags\n\t" \
			"movb %%ah,_regflags+1\n\t" \
			:"=r" (v) : "rmi" (s), "0" (d) : "%eax","cc","memory"); \
	regflags.x = regflags.cznv; \
    } while (0)
#define optflag_addw(v, s, d) do { \
  __asm__ __volatile__ ("addw %w1,%w0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,_regflags\n\t" \
			"movb %%ah,_regflags+1\n\t" \
			: "=r" (v) : "rmi" (s), "0" (d) : "%eax","cc","memory"); \
	regflags.x = regflags.cznv; \
    } while (0)

#define optflag_addb(v, s, d) do { \
  __asm__ __volatile__ ("addb %b1,%b0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,_regflags\n\t" \
			"movb %%ah,_regflags+1\n\t" \
			:"=q" (v) : "qmi" (s), "0" (d) : "%eax","cc","memory"); \
	regflags.x = regflags.cznv; \
    } while (0)

#define optflag_subl(v, s, d) do { \
  __asm__ __volatile__ ("subl %k1,%k0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,_regflags\n\t" \
			"movb %%ah,_regflags+1\n\t" \
			: "=r" (v) : "rmi" (s), "0" (d) : "%eax","cc","memory"); \
	regflags.x = regflags.cznv; \
    } while (0)

#define optflag_subw(v, s, d) do { \
  __asm__ __volatile__ ("subw %w1,%w0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,_regflags\n\t" \
			"movb %%ah,_regflags+1\n\t" \
			: "=r" (v) : "rmi" (s), "0" (d) : "%eax","cc","memory"); \
	regflags.x = regflags.cznv; \
    } while (0)

#define optflag_subb(v, s, d) do { \
   __asm__ __volatile__ ("subb %b1,%b0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,_regflags\n\t" \
			"movb %%ah,_regflags+1\n\t" \
			: "=q" (v) : "qmi" (s), "0" (d) : "%eax","cc","memory"); \
	regflags.x = regflags.cznv; \
    } while (0)

#define optflag_cmpl(s, d) \
  __asm__ __volatile__ ("cmpl %k0,%k1\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,_regflags\n\t" \
			"movb %%ah,_regflags+1\n\t" \
			:: "rmi" (s), "r" (d) : "%eax","cc","memory")

#define optflag_cmpw(s, d) \
  __asm__ __volatile__ ("cmpw %w0,%w1\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,_regflags\n\t" \
			"movb %%ah,_regflags+1\n\t" \
			:: "rmi" (s), "r" (d) : "%eax","cc","memory");

#define optflag_cmpb(s, d) \
  __asm__ __volatile__ ("cmpb %b0,%b1\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,_regflags\n\t" \
			"movb %%ah,_regflags+1\n\t" \
			:: "qmi" (s), "q" (d) : "%eax","cc","memory")
#else /*ifdef apple*/
#define optflag_testl(v) \
  __asm__ __volatile__ ("andl %0,%0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,regflags\n\t" \
			"movb %%ah,regflags+1\n\t" \
			:: "r" (v) : "%eax","cc","memory")
#define optflag_testw(v) \
  __asm__ __volatile__ ("andw %w0,%w0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,regflags\n\t" \
			"movb %%ah,regflags+1\n\t" \
			:: "r" (v) : "%eax","cc","memory")

#define optflag_testb(v) \
  __asm__ __volatile__ ("andb %b0,%b0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,regflags\n\t" \
			"movb %%ah,regflags+1\n\t" \
			:: "q" (v) : "%eax","cc","memory")

#define optflag_addl(v, s, d) do { \
  __asm__ __volatile__ ("addl %k1,%k0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,regflags\n\t" \
			"movb %%ah,regflags+1\n\t" \
			:"=r" (v) : "rmi" (s), "0" (d) : "%eax","cc","memory"); \
	regflags.x = regflags.cznv; \
    } while (0)
#define optflag_addw(v, s, d) do { \
  __asm__ __volatile__ ("addw %w1,%w0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,regflags\n\t" \
			"movb %%ah,regflags+1\n\t" \
			: "=r" (v) : "rmi" (s), "0" (d) : "%eax","cc","memory"); \
	regflags.x = regflags.cznv; \
    } while (0)

#define optflag_addb(v, s, d) do { \
  __asm__ __volatile__ ("addb %b1,%b0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,regflags\n\t" \
			"movb %%ah,regflags+1\n\t" \
			:"=q" (v) : "qmi" (s), "0" (d) : "%eax","cc","memory"); \
	regflags.x = regflags.cznv; \
    } while (0)

#define optflag_subl(v, s, d) do { \
  __asm__ __volatile__ ("subl %k1,%k0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,regflags\n\t" \
			"movb %%ah,regflags+1\n\t" \
			: "=r" (v) : "rmi" (s), "0" (d) : "%eax","cc","memory"); \
	regflags.x = regflags.cznv; \
    } while (0)

#define optflag_subw(v, s, d) do { \
  __asm__ __volatile__ ("subw %w1,%w0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,regflags\n\t" \
			"movb %%ah,regflags+1\n\t" \
			: "=r" (v) : "rmi" (s), "0" (d) : "%eax","cc","memory"); \
	regflags.x = regflags.cznv; \
    } while (0)

#define optflag_subb(v, s, d) do { \
   __asm__ __volatile__ ("subb %b1,%b0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,regflags\n\t" \
			"movb %%ah,regflags+1\n\t" \
			: "=q" (v) : "qmi" (s), "0" (d) : "%eax","cc","memory"); \
	regflags.x = regflags.cznv; \
    } while (0)

#define optflag_cmpl(s, d) \
  __asm__ __volatile__ ("cmpl %k0,%k1\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,regflags\n\t" \
			"movb %%ah,regflags+1\n\t" \
			:: "rmi" (s), "r" (d) : "%eax","cc","memory")

#define optflag_cmpw(s, d) \
  __asm__ __volatile__ ("cmpw %w0,%w1\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,regflags\n\t" \
			"movb %%ah,regflags+1\n\t" \
			:: "rmi" (s), "r" (d) : "%eax","cc","memory");

#define optflag_cmpb(s, d) \
  __asm__ __volatile__ ("cmpb %b0,%b1\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			"movb %%al,regflags\n\t" \
			"movb %%ah,regflags+1\n\t" \
			:: "qmi" (s), "q" (d) : "%eax","cc","memory")
#endif /*ifdef apple*/

#endif

#else
/*
#define FLAGBIT_N	31
#define FLAGBIT_Z	30
#define FLAGBIT_C	29
#define FLAGBIT_V	28
#define FLAGBIT_X	29
*/
/* ARM COND
31 30 29 28
N  Z  C  V
*/
#if ARM_OPT_TEST

#define optflag_testl(v) \
         __asm__ __volatile__ (  \
		"adds %1,%1,#0 \n"\
  		"mrs   %0, cpsr  \n"\
		: "=r" (regflags.cznv) \
		: "r" (v)\
		: "cc"\
		);

#define optflag_testw(v) optflag_testl((uae_s32)(v))
#define optflag_testb(v) optflag_testl((uae_s32)(v))

#define optflag_addlong(v, s, d) \
         __asm__ __volatile__ ( \
        	"adds %1,%2,%3 \n"\
  		"mrs   %0, cpsr  \n"\
		: "=r" (regflags.cznv), "=r" (v) \
		: "r" (s), "r" (d) \
		: "cc"\
		);	

#define optflag_addl(v, s, d) do { optflag_addlong(v, s, d); regflags.x = regflags.cznv; } while (0)
#define optflag_addw(v, s, d) do { optflag_addlong(v, (s) << 16, (d) << 16); v = v >> 16; regflags.x = regflags.cznv; } while (0)
#define optflag_addb(v, s, d) do { optflag_addlong(v, (s) << 24, (d) << 24); v = v >> 24; regflags.x = regflags.cznv; } while (0)

#define optflag_sublong(v, s, d) \
	__asm__ __volatile__ (\
		"rsbs  %1, %2, %3\n\t" \
  		"mrs   %0, cpsr   \n"\
		"eor %0,%0,#0x20000000 \n"\
		: "=r" (regflags.cznv), "=r" (v) \
		: "r" (s), "r" (d) \
		: "cc"\
	);

#define optflag_subl(v, s, d) do { optflag_sublong(v, s, d); regflags.x = regflags.cznv; } while (0)
#define optflag_subw(v, s, d) do { optflag_sublong(v, (s) << 16, (d) << 16); v = v >> 16; regflags.x = regflags.cznv; } while (0)
#define optflag_subb(v, s, d) do { optflag_sublong(v, (s) << 24, (d) << 24); v = v >> 24; regflags.x = regflags.cznv; } while (0)

#define  optflag_cmplong(s,d) \
	register int tmp;\
    	__asm__ __volatile__ (  \
	       "rsbs   	%1, %2,%3		\n"\
               "mrs     %0, cpsr 	 	\n"\
	       "eor %0,%0,#0x20000000 \n"\
 		:"=r"(regflags.cznv) ,"=r"(tmp)	\
       		:"r" (s), "r" (d)\
             	:"cc"      	\
         ); 

#define optflag_cmpl(s, d) optflag_cmplong(s, d)
#define optflag_cmpw(s, d) optflag_cmplong((s) << 16, (d) << 16)
#define optflag_cmpb(s, d) optflag_cmplong((s) << 24, (d) << 24)


#else

#define FLAGBIT_N	15
#define FLAGBIT_Z	14
#define FLAGBIT_C	8
#define FLAGBIT_V	0
#define FLAGBIT_X	8

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
#define IOR_CZNV(X)	(regflags.cznv |= (X))
#define SET_CZNV(X)	(regflags.cznv  = (X))

#define COPY_CARRY() (regflags.x = regflags.cznv)

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

#endif

#endif 

#endif /* EUAE_MACHDEP_M68KOPS_H */
