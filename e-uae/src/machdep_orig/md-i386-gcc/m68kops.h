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

#define optflag_testl(regs, v)				\
	asm (						\
		"andl %1,%1		\n\t"		\
		"lahf			\n\t"		\
		"seto %%al		\n\t"		\
		"movb %%ah, 65(%2)	\n\t"		\
		"movb %%al, 64(%2)	\n\t"		\
							\
		: "=m" ((regs)->ccrflags.cznv)		\
		: "q" (v), "r" (regs)			\
		: "eax", "cc"				\
	)

#define optflag_testw(regs, v)				\
	asm (						\
		"andw %w1,%w1		\n\t"		\
		"lahf			\n\t"		\
		"seto %%al		\n\t"		\
		"movb %%ah, 65(%2)	\n\t"		\
		"movb %%al, 64(%2)	\n\t"		\
							\
		: "=m" ((regs)->ccrflags.cznv)		\
		: "q" (v), "r" (regs)			\
		: "eax", "cc"				\
	)

#define optflag_testb(regs, v)				\
	asm ( 						\
		"andb %b1,%b1		\n\t"		\
		"lahf			\n\t"		\
		"seto %%al		\n\t"		\
		"movb %%ah, 65(%2)	\n\t"		\
		"movb %%al, 64(%2)	\n\t"		\
							\
		: "=m" ((regs)->ccrflags.cznv)		\
		: "q" (v), "r" (regs)			\
		: "eax", "cc"				\
	)

/*
 * Add operations
 *
 * Perform v = s + d and set ZNCV and X accordingly
 */
#define optflag_addl(regs, v, s, d)			\
    do {						\
	asm (						\
		"addl %k2,%k1		\n\t"		\
		"lahf			\n\t"		\
		"seto %%al		\n\t"		\
		"movb %%ah, 69(%4)	\n\t"		\
		"movb %%ah, 65(%4)	\n\t"		\
		"movb %%al, 64(%4)	\n\t"		\
							\
		: "=m" ((regs)->ccrflags),		\
		  "=r" (v)				\
		: "rmi" (s), "1" (d), "r" (regs)	\
		: "cc", "eax"				\
	);						\
    } while (0)

#define optflag_addw(regs, v, s, d) \
    do { \
	asm ( \
		"addw %w2,%w1		\n\t"		\
		"lahf			\n\t"		\
		"seto %%al		\n\t"		\
		"movb %%ah, 69(%4)	\n\t"		\
		"movb %%ah, 65(%4)	\n\t"		\
		"movb %%al, 64(%4)	\n\t"		\
							\
		: "=m" ((regs)->ccrflags),		\
		  "=r" (v)				\
		: "rmi" (s), "1" (d), "r" (regs)	\
		: "cc", "eax"				\
	);						\
    } while (0)

#define optflag_addb(regs, v, s, d) \
    do { \
	asm ( \
		"addb %b2,%b1		\n\t"		\
		"lahf			\n\t"		\
		"seto %%al		\n\t"		\
		"movb %%ah, 69(%4)	\n\t"		\
		"movb %%ah, 65(%4)	\n\t"		\
		"movb %%al, 64(%4)	\n\t"		\
							\
		: "=m" ((regs)->ccrflags),		\
		  "=q" (v)				\
		: "qmi" (s), "1" (d), "r" (regs)	\
		: "cc", "eax"				\
	);						\
    } while (0)


/*
 * Subtraction operations
 *
 * Perform v = d - s and set ZNCV and X accordingly
 */
#define optflag_subl(regs, v, s, d)			\
    do {						\
	asm (						\
		"subl %k2,%k1		\n\t"		\
		"lahf			\n\t"		\
		"seto %%al		\n\t"		\
		"movb %%ah, 69(%4)	\n\t"		\
		"movb %%ah, 65(%4)	\n\t"		\
		"movb %%al, 64(%4)	\n\t"		\
							\
		: "=m" ((regs)->ccrflags),		\
		  "=r" (v)				\
		: "rmi" (s), "1" (d), "r" (regs)	\
		: "cc", "eax"				\
	);						\
    } while (0)

#define optflag_subw(regs, v, s, d)			\
    do {						\
	asm (						\
		"subw %w2,%w1		\n\t"		\
		"lahf			\n\t"		\
		"seto %%al		\n\t"		\
		"movb %%ah, 69(%4)	\n\t"		\
		"movb %%ah, 65(%4)	\n\t"		\
		"movb %%al, 64(%4)	\n\t"		\
							\
		: "=m" ((regs)->ccrflags),		\
		  "=r" (v)				\
		: "rmi" (s), "1" (d), "r" (regs)	\
		: "cc", "eax"				\
	);						\
    } while (0)

#define optflag_subb(regs, v, s, d)			\
    do {						\
	asm (						\
		"subb %b2,%b1		\n\t"		\
		"lahf			\n\t"		\
		"seto %%al		\n\t"		\
		"movb %%ah, 69(%4)	\n\t"		\
		"movb %%ah, 65(%4)	\n\t"		\
		"movb %%al, 64(%4)	\n\t"		\
							\
		: "=m" ((regs)->ccrflags),		\
		  "=q" (v)				\
		: "qmi" (s), "1" (d), "r" (regs)	\
		: "cc", "eax"				\
	);						\
    } while (0)

/*
 * Compare operations
 *
 * Evaluate d - s and set ZNCV accordingly
 */
#define optflag_cmpl(regs, s, d)			\
	asm (						\
		"cmpl %k1,%k2		\n\t"		\
		"lahf			\n\t"		\
		"seto %%al		\n\t"		\
		"movb %%ah, 65(%3)	\n\t"		\
		"movb %%al, 64(%3)	\n\t"		\
							\
		: "=m" ((regs)->ccrflags.cznv)		\
		: "rmi" (s), "r" (d), "r" (regs)	\
		: "eax", "cc"				\
	)

#define optflag_cmpw(regs, s, d)			\
	asm (						\
		"cmpw %w1,%w2		\n\t"		\
		"lahf			\n\t"		\
		"seto %%al		\n\t"		\
		"movb %%ah, 65(%3)	\n\t"		\
		"movb %%al, 64(%3)	\n\t"		\
							\
		: "=m" ((regs)->ccrflags.cznv)		\
		: "rmi" (s), "r" (d), "r" (regs)	\
		: "eax", "cc"				\
	)

#define optflag_cmpb(regs, s, d)			\
	asm (						\
		"cmpb %b1,%b2		\n\t"		\
		"lahf			\n\t"		\
		"seto %%al		\n\t"		\
		"movb %%ah, 65(%3)	\n\t"		\
		"movb %%al, 64(%3)	\n\t"		\
							\
		: "=m" ((regs)->ccrflags.cznv)		\
		: "qmi" (s), "q" (d), "r" (regs)	\
		: "eax", "cc"				\
	)

#endif /* EUAE_MACHDEP_M68KOPS_H */
