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

#endif /* EUAE_MACHDEP_M68KOPS_H */
