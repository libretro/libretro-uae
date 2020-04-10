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
#define optflag_testl(v) \
  __asm__ __volatile__ ("andl %1,%1\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			: "=a"(regflags.cznv) : "r" (v) : "cc","memory")
#define optflag_testw(v) \
  __asm__ __volatile__ ("andw %w1,%w1\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			: "=a"(regflags.cznv) : "r" (v) : "cc","memory")

#define optflag_testb(v) \
  __asm__ __volatile__ ("andb %b1,%b1\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			: "=a"(regflags.cznv) : "q" (v) : "cc","memory")

#define optflag_addl(v, s, d) do { \
  __asm__ __volatile__ ("addl %k2,%k0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			:"=r" (v), "=a"(regflags.cznv) : "rmi" (s), "0" (d) : "cc","memory"); \
	regflags.x = regflags.cznv; \
    } while (0)
#define optflag_addw(v, s, d) do { \
  __asm__ __volatile__ ("addw %w2,%w0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			: "=r" (v), "=a"(regflags.cznv) : "rmi" (s), "0" (d) : "cc","memory"); \
	regflags.x = regflags.cznv; \
    } while (0)

#define optflag_addb(v, s, d) do { \
  __asm__ __volatile__ ("addb %b2,%b0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			:"=q" (v), "=a"(regflags.cznv) : "qmi" (s), "0" (d) : "cc","memory"); \
	regflags.x = regflags.cznv; \
    } while (0)

#define optflag_subl(v, s, d) do { \
  __asm__ __volatile__ ("subl %k2,%k0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			: "=r" (v), "=a"(regflags.cznv) : "rmi" (s), "0" (d) : "cc","memory"); \
	regflags.x = regflags.cznv; \
    } while (0)

#define optflag_subw(v, s, d) do { \
  __asm__ __volatile__ ("subw %w2,%w0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			: "=r" (v), "=a"(regflags.cznv) : "rmi" (s), "0" (d) : "cc","memory"); \
	regflags.x = regflags.cznv; \
    } while (0)

#define optflag_subb(v, s, d) do { \
   __asm__ __volatile__ ("subb %b2,%b0\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			: "=q" (v), "=a"(regflags.cznv) : "qmi" (s), "0" (d) : "cc","memory"); \
	regflags.x = regflags.cznv; \
    } while (0)

#define optflag_cmpl(s, d) \
  __asm__ __volatile__ ("cmpl %k1,%k2\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			: "=a"(regflags.cznv) : "rmi" (s), "r" (d) : "cc","memory")

#define optflag_cmpw(s, d) \
  __asm__ __volatile__ ("cmpw %w1,%w2\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			: "=a"(regflags.cznv) : "rmi" (s), "r" (d) : "cc","memory");

#define optflag_cmpb(s, d) \
  __asm__ __volatile__ ("cmpb %b1,%b2\n\t" \
			"lahf\n\t" \
			"seto %%al\n\t" \
			: "=a"(regflags.cznv) : "qmi" (s), "q" (d) : "cc","memory")

#endif /* EUAE_MACHDEP_M68KOPS_H */
