/*
 * E-UAE - The portable Amiga Emulator
 *
 * MC68000 emulation - machine-dependent optimized operations
 *
 * (c) 2004-2005 Richard Drummond
 *
 * Based on code from UAE.
 * Copyright 1996 Bernd Schmidt, Samuel Devulder
 */

#ifndef EUAE_MACHDEP_M68KOPS_H
#define EUAE_MACHDEP_M68KOPS_H

/* sam: MIT or MOTOROLA syntax ? */
#ifdef __linux__
# define MITMOT(mit,mot) mot
# define MIT(x)
# define MOT(x) x
#else /* AmigaOS */
# define MITMOT(mit,mot) mit
# define MIT(x) x
# define MOT(x)
#endif

/*
 * Test operations
 *
 * Evaluate operand and set Z and N flags. Always clear C and V.
 */
#define m68k_flag_tst(regs, l, v)			\
    asm (						\
	MIT(	"tst"#l" %1		\n\t"		\
		"movew ccr, %0          \n\t"		\
	)						\
	MOT(	"tst."#l" %1		\n\t"		\
		"move.w %%ccr, %0	\n\t"		\
	)						\
	: "=m"  ((regs)->ccrflags)			\
	: "dmi" (v) 					\
	: "cc"						\
    )

#define optflag_testl(regs, v)		m68k_flag_tst (regs, l, v)
#define optflag_testw(regs, v)		m68k_flag_tst (regs, w, v)
#define optflag_testb(regs, v)		m68k_flag_tst (regs, b, v)

/*
 * Add operations
 *
 * Perform v = s + d and set ZNCV accordingly
 */
#define m68k_flag_add(regs, l, v, s, d)			\
    do {						\
	asm (						\
	    MIT("add"#l" %3,%1		\n\t"		\
		"movew ccr,%0		\n\t"		\
	    )						\
	    MOT("add."#l" %3,%1	\n\t"			\
		"move.w %%ccr,%0	\n\t"		\
	    )						\
	    : "=dm" ((regs)->ccrflags),			\
	      "=&d" (v)					\
	    : "1"   (s),				\
	      "dmi" (d)					\
	    : "cc"					\
	);						\
	COPY_CARRY (&(regs)->ccrflags);			\
    } while (0)

#define optflag_addl(regs, v, s, d)	m68k_flag_add (regs, l, v, s, d)
#define optflag_addw(regs, v, s, d)	m68k_flag_add (regs, w, v, s, d)
#define optflag_addb(regs, v, s, d)	m68k_flag_add (regs, b, v, s, d)

/*
 * Subtraction operations
 *
 * Perform v = d - s and set ZNCV accordingly
 */
#define m68k_flag_sub(regs, l, v, s, d)			\
    do {						\
	asm (						\
	    MIT("sub"#l" %2,%1		\n\t"		\
		"movew ccr,%0		\n\t"		\
	    )						\
	    MOT("sub."#l" %2,%1		\n\t"		\
		"move.w %%ccr,%0	\n\t"		\
	    )						\
	    : "=dm" ((regs)->ccrflags),			\
	      "=&d" (v)					\
	    : "dmi" (s),				\
	      "1"   (d)					\
	    : "cc"					\
	 );						\
	COPY_CARRY (&(regs)->ccrflags);			\
    } while (0)

#define optflag_subl(regs, v, s, d)	m68k_flag_sub (regs, l, v, s, d)
#define optflag_subw(regs, v, s, d)	m68k_flag_sub (regs, w, v, s, d)
#define optflag_subb(regs, v, s, d)	m68k_flag_sub (regs, b, v, s, d)

/*
 * Compare operations
 *
 * Perform d - s and set ZNCV accordingly
 */
#define m68k_flag_cmp(regs, l, s, d)			\
    asm (						\
	MIT(	"cmp"#l" %1,%2		\n\t"		\
		"movew ccr, %0          \n\t"		\
	)						\
	MOT(	"cmp."#l" %0,%1		\n\t"		\
		"move.w %%ccr, %0	\n\t"		\
	)						\
	: "=m"  ((regs)->ccrflags)			\
	: "dmi" (s), 					\
	  "d"   (d)					\
	: "cc"						\
    )

#define optflag_cmpl(regs, s, d)	m68k_flag_cmp (regs, l, s, d)
#define optflag_cmpw(regs, s, d)	m68k_flag_cmp (regs, w, s, d)
#define optflag_cmpb(regs, s, d)	m68k_flag_cmp (regs, b, s, d)

#endif /* EUAE_MACHDEP_M68KOPS_H */
