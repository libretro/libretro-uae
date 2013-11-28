#include "sysconfig.h"
#include "sysdeps.h"
#include "options.h"
#include "memory_uae.h"
#include "custom.h"
#include "events.h"
#include "newcpu.h"
#include "machdep/m68kops.h"
#include "cpu_prefetch.h"
#include "cputbl.h"
#include "cpummu.h"
#define CPUFUNC(x) x##_ff
#define SET_CFLG_ALWAYS(x) SET_CFLG(x)
#define SET_NFLG_ALWAYS(x) SET_NFLG(x)
#ifdef NOFLAGS
#include "noflags.h"
#endif

#if !defined(PART_1) && !defined(PART_2) && !defined(PART_3) && !defined(PART_4) && !defined(PART_5) && !defined(PART_6) && !defined(PART_7) && !defined(PART_8)
#define PART_1 1
#define PART_2 1
#define PART_3 1
#define PART_4 1
#define PART_5 1
#define PART_6 1
#define PART_7 1
#define PART_8 1
#endif

#ifdef PART_1
/* OR.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0000_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0010_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 20 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0018_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0020_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0028_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0030_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0038_33)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0039_33)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (8);
return 28 * CYCLE_UNIT / 2;
}

/* ORSR.B #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_003c_33)(uae_u32 opcode)
{
{	MakeSR ();
{	uae_s16 src = get_iword_mmu060 (2);
	src &= 0xFF;
	regs.sr |= src;
	MakeFromSR ();
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0040_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0050_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0058_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0060_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0068_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0070_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0078_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0079_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* ORSR.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_007c_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel14614; }
{	MakeSR ();
{	uae_s16 src = get_iword_mmu060 (2);
	regs.sr |= src;
	MakeFromSR ();
}}}	m68k_incpci (4);
endlabel14614: ;
return 8 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0080_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0090_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0098_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_00a0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 30 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_00a8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (6);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (8);
return 32 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_00b0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}}return 32 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_00b8_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (6);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (8);
return 32 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_00b9_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (6);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (10);
return 36 * CYCLE_UNIT / 2;
}

/* CHK2.B #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_00d0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s8)get_byte_mmu060 (dsta); upper = (uae_s32)(uae_s8)get_byte_mmu060 (dsta + 1);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel14623; }
}
}}}	m68k_incpci (4);
endlabel14623: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.B #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_00e8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s8)get_byte_mmu060 (dsta); upper = (uae_s32)(uae_s8)get_byte_mmu060 (dsta + 1);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel14624; }
}
}}}	m68k_incpci (6);
endlabel14624: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.B #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_00f0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s8)get_byte_mmu060 (dsta); upper = (uae_s32)(uae_s8)get_byte_mmu060 (dsta + 1);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel14625; }
}
}}}}endlabel14625: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.B #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_00f8_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s8)get_byte_mmu060 (dsta); upper = (uae_s32)(uae_s8)get_byte_mmu060 (dsta + 1);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel14626; }
}
}}}	m68k_incpci (6);
endlabel14626: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.B #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_00f9_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s8)get_byte_mmu060 (dsta); upper = (uae_s32)(uae_s8)get_byte_mmu060 (dsta + 1);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel14627; }
}
}}}	m68k_incpci (8);
endlabel14627: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.B #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_00fa_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu060 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s8)get_byte_mmu060 (dsta); upper = (uae_s32)(uae_s8)get_byte_mmu060 (dsta + 1);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel14628; }
}
}}}	m68k_incpci (6);
endlabel14628: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.B #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_00fb_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s8)get_byte_mmu060 (dsta); upper = (uae_s32)(uae_s8)get_byte_mmu060 (dsta + 1);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel14629; }
}
}}}}endlabel14629: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BTST.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0100_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= 31;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* MVPMR.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0108_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr memp = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_u16 val = (get_byte_mmu060 (memp) << 8) + get_byte_mmu060 (memp + 2);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0110_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0118_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0120_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0128_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0130_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0138_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0139_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s8 dst = get_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_013a_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_getpc () + 2;
	dsta += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_013b_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 dst = get_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,#<data>.B */
uae_u32 REGPARAM2 CPUFUNC(op_013c_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = get_ibyte_mmu060 (2);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* BCHG.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0140_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= 31;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	m68k_dreg (regs, dstreg) = (dst);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* MVPMR.L (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0148_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr memp = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_u32 val = (get_byte_mmu060 (memp) << 24) + (get_byte_mmu060 (memp + 2) << 16)
              + (get_byte_mmu060 (memp + 4) << 8) + get_byte_mmu060 (memp + 6);
	m68k_dreg (regs, dstreg) = (val);
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0150_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0158_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0160_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0168_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0170_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0178_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0179_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_017a_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_getpc () + 2;
	dsta += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_017b_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* BCLR.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0180_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= 31;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	m68k_dreg (regs, dstreg) = (dst);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* MVPRM.W Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0188_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
	uaecptr memp = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	put_byte_mmu060 (memp, src >> 8);
	put_byte_mmu060 (memp + 2, src);
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0190_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0198_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_01a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_01a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_01b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_01b8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_01b9_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_01ba_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_getpc () + 2;
	dsta += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_01bb_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* BSET.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_01c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= 31;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	m68k_dreg (regs, dstreg) = (dst);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* MVPRM.L Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_01c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
	uaecptr memp = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	put_byte_mmu060 (memp, src >> 24);
	put_byte_mmu060 (memp + 2, src >> 16);
	put_byte_mmu060 (memp + 4, src >> 8);
	put_byte_mmu060 (memp + 6, src);
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_01d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_01d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_01e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_01e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_01f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_01f8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_01f9_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_01fa_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_getpc () + 2;
	dsta += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_01fb_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0200_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0210_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 20 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0218_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0220_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0228_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0230_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0238_33)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0239_33)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (8);
return 28 * CYCLE_UNIT / 2;
}

/* ANDSR.B #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_023c_33)(uae_u32 opcode)
{
{	MakeSR ();
{	uae_s16 src = get_iword_mmu060 (2);
	src |= 0xFF00;
	regs.sr &= src;
	MakeFromSR ();
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0240_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0250_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0258_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0260_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0268_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0270_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0278_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0279_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* ANDSR.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_027c_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel14692; }
{	MakeSR ();
{	uae_s16 src = get_iword_mmu060 (2);
	regs.sr &= src;
	MakeFromSR ();
}}}	m68k_incpci (4);
endlabel14692: ;
return 8 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0280_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0290_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0298_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_02a0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 30 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_02a8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (6);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (8);
return 32 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_02b0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}}return 32 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_02b8_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (6);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (8);
return 32 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_02b9_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (6);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (10);
return 36 * CYCLE_UNIT / 2;
}

/* CHK2.W #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_02d0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s16)get_word_mmu060 (dsta); upper = (uae_s32)(uae_s16)get_word_mmu060 (dsta + 2);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel14701; }
}
}}}	m68k_incpci (4);
endlabel14701: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.W #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_02e8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s16)get_word_mmu060 (dsta); upper = (uae_s32)(uae_s16)get_word_mmu060 (dsta + 2);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel14702; }
}
}}}	m68k_incpci (6);
endlabel14702: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.W #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_02f0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s16)get_word_mmu060 (dsta); upper = (uae_s32)(uae_s16)get_word_mmu060 (dsta + 2);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel14703; }
}
}}}}endlabel14703: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.W #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_02f8_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s16)get_word_mmu060 (dsta); upper = (uae_s32)(uae_s16)get_word_mmu060 (dsta + 2);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel14704; }
}
}}}	m68k_incpci (6);
endlabel14704: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.W #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_02f9_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s16)get_word_mmu060 (dsta); upper = (uae_s32)(uae_s16)get_word_mmu060 (dsta + 2);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel14705; }
}
}}}	m68k_incpci (8);
endlabel14705: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.W #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_02fa_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu060 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s16)get_word_mmu060 (dsta); upper = (uae_s32)(uae_s16)get_word_mmu060 (dsta + 2);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel14706; }
}
}}}	m68k_incpci (6);
endlabel14706: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.W #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_02fb_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s16)get_word_mmu060 (dsta); upper = (uae_s32)(uae_s16)get_word_mmu060 (dsta + 2);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel14707; }
}
}}}}endlabel14707: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* SUB.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0400_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0410_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 20 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0418_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0420_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0428_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0430_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0438_33)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0439_33)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (8);
return 28 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0440_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0450_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0458_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0460_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0468_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0470_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0478_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0479_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0480_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0490_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0498_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_04a0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 30 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_04a8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (6);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (8);
return 32 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_04b0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}}return 32 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_04b8_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (6);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (8);
return 32 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_04b9_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (6);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (10);
return 36 * CYCLE_UNIT / 2;
}

/* CHK2.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_04d0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = get_long_mmu060 (dsta); upper = get_long_mmu060 (dsta + 4);
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel14732; }
}
}}}	m68k_incpci (4);
endlabel14732: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_04e8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = get_long_mmu060 (dsta); upper = get_long_mmu060 (dsta + 4);
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel14733; }
}
}}}	m68k_incpci (6);
endlabel14733: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_04f0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = get_long_mmu060 (dsta); upper = get_long_mmu060 (dsta + 4);
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel14734; }
}
}}}}endlabel14734: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_04f8_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = get_long_mmu060 (dsta); upper = get_long_mmu060 (dsta + 4);
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel14735; }
}
}}}	m68k_incpci (6);
endlabel14735: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_04f9_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = get_long_mmu060 (dsta); upper = get_long_mmu060 (dsta + 4);
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel14736; }
}
}}}	m68k_incpci (8);
endlabel14736: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.L #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_04fa_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu060 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = get_long_mmu060 (dsta); upper = get_long_mmu060 (dsta + 4);
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel14737; }
}
}}}	m68k_incpci (6);
endlabel14737: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.L #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_04fb_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = get_long_mmu060 (dsta); upper = get_long_mmu060 (dsta + 4);
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel14738; }
}
}}}}endlabel14738: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* ADD.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0600_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0610_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 20 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0618_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0620_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0628_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0630_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0638_33)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0639_33)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (8);
return 28 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0640_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0650_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0658_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0660_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0668_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0670_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0678_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0679_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0680_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0690_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0698_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_06a0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 30 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_06a8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (6);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (8);
return 32 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_06b0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}}return 32 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_06b8_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (6);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (8);
return 32 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_06b9_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (6);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (10);
return 36 * CYCLE_UNIT / 2;
}

/* RTM.L Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* RTM.L An */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* CALLM.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* CALLM.L (d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* CALLM.L (d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* CALLM.L (xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06f8_33)(uae_u32 opcode)
{
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* CALLM.L (xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06f9_33)(uae_u32 opcode)
{
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* CALLM.L (d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06fa_33)(uae_u32 opcode)
{
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* CALLM.L (d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06fb_33)(uae_u32 opcode)
{
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* BTST.L #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0800_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= 31;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0810_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0818_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0820_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0828_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0830_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0838_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0839_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_s8 dst = get_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (8);
return 20 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_083a_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_083b_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 dst = get_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,#<data>.B */
uae_u32 REGPARAM2 CPUFUNC(op_083c_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s8 dst = get_ibyte_mmu060 (4);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* BCHG.L #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0840_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= 31;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	m68k_dreg (regs, dstreg) = (dst);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0850_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0858_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0860_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0868_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0870_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0878_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0879_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_087a_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_087b_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* BCLR.L #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0880_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= 31;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	m68k_dreg (regs, dstreg) = (dst);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0890_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0898_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_08a0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_08a8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_08b0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_08b8_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_08b9_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_08ba_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_08bb_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* BSET.L #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_08c0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= 31;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	m68k_dreg (regs, dstreg) = (dst);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_08d0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_08d8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_08e0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_08e8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_08f0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_08f8_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_08f9_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_08fa_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_08fb_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_rmw_byte_mmu060 (dsta, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0a00_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0a10_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 20 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0a18_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0a20_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0a28_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0a30_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0a38_33)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0a39_33)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (8);
return 28 * CYCLE_UNIT / 2;
}

/* EORSR.B #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_0a3c_33)(uae_u32 opcode)
{
{	MakeSR ();
{	uae_s16 src = get_iword_mmu060 (2);
	src &= 0xFF;
	regs.sr ^= src;
	MakeFromSR ();
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0a40_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0a50_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0a58_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0a60_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0a68_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0a70_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0a78_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0a79_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* EORSR.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_0a7c_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel14830; }
{	MakeSR ();
{	uae_s16 src = get_iword_mmu060 (2);
	regs.sr ^= src;
	MakeFromSR ();
}}}	m68k_incpci (4);
endlabel14830: ;
return 8 * CYCLE_UNIT / 2;
}

/* EOR.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0a80_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif

#ifdef PART_2
/* EOR.L #<data>.L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0a90_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* EOR.L #<data>.L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0a98_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

/* EOR.L #<data>.L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0aa0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 30 * CYCLE_UNIT / 2;
}

/* EOR.L #<data>.L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0aa8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (6);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (8);
return 32 * CYCLE_UNIT / 2;
}

/* EOR.L #<data>.L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0ab0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}}return 32 * CYCLE_UNIT / 2;
}

/* EOR.L #<data>.L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0ab8_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (6);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (8);
return 32 * CYCLE_UNIT / 2;
}

/* EOR.L #<data>.L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0ab9_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (6);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (10);
return 36 * CYCLE_UNIT / 2;
}

/* CAS.B #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ad0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_lrmw_byte_mmu060 (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpb ((uae_s8)(m68k_dreg (regs, rc)), (uae_s8)(dst));
	if (GET_ZFLG ()){
		put_lrmw_byte_mmu060 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_byte_mmu060 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xff) | (dst & 0xff);
}}}}}}	m68k_incpci (4);
return 20 * CYCLE_UNIT / 2;
}

#endif
/* CAS.B #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ad8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_lrmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpb ((uae_s8)(m68k_dreg (regs, rc)), (uae_s8)(dst));
	if (GET_ZFLG ()){
		put_lrmw_byte_mmu060 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_byte_mmu060 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xff) | (dst & 0xff);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

#endif
/* CAS.B #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ae0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_lrmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpb ((uae_s8)(m68k_dreg (regs, rc)), (uae_s8)(dst));
	if (GET_ZFLG ()){
		put_lrmw_byte_mmu060 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_byte_mmu060 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xff) | (dst & 0xff);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

#endif
/* CAS.B #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ae8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_lrmw_byte_mmu060 (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpb ((uae_s8)(m68k_dreg (regs, rc)), (uae_s8)(dst));
	if (GET_ZFLG ()){
		put_lrmw_byte_mmu060 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_byte_mmu060 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xff) | (dst & 0xff);
}}}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

#endif
/* CAS.B #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0af0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_lrmw_byte_mmu060 (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpb ((uae_s8)(m68k_dreg (regs, rc)), (uae_s8)(dst));
	if (GET_ZFLG ()){
		put_lrmw_byte_mmu060 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_byte_mmu060 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xff) | (dst & 0xff);
}}}}}}}return 24 * CYCLE_UNIT / 2;
}

#endif
/* CAS.B #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0af8_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_lrmw_byte_mmu060 (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpb ((uae_s8)(m68k_dreg (regs, rc)), (uae_s8)(dst));
	if (GET_ZFLG ()){
		put_lrmw_byte_mmu060 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_byte_mmu060 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xff) | (dst & 0xff);
}}}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

#endif
/* CAS.B #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0af9_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_s8 dst = get_lrmw_byte_mmu060 (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpb ((uae_s8)(m68k_dreg (regs, rc)), (uae_s8)(dst));
	if (GET_ZFLG ()){
		put_lrmw_byte_mmu060 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_byte_mmu060 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xff) | (dst & 0xff);
}}}}}}	m68k_incpci (8);
return 28 * CYCLE_UNIT / 2;
}

#endif
/* CMP.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0c00_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0c10_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu060 (dsta);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0c18_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0c20_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0c28_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_byte_mmu060 (dsta);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0c30_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu060 (dsta);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0c38_33)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_byte_mmu060 (dsta);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0c39_33)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_s8 dst = get_byte_mmu060 (dsta);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0c3a_33)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 dst = get_byte_mmu060 (dsta);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

#endif
/* CMP.B #<data>.B,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0c3b_33)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 dst = get_byte_mmu060 (dsta);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* CMP.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0c40_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0c50_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu060 (dsta);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0c58_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0c60_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0c68_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s16 dst = get_word_mmu060 (dsta);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0c70_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu060 (dsta);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0c78_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s16 dst = get_word_mmu060 (dsta);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0c79_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_s16 dst = get_word_mmu060 (dsta);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (8);
return 20 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0c7a_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s16 dst = get_word_mmu060 (dsta);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CMP.W #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0c7b_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 dst = get_word_mmu060 (dsta);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}return 16 * CYCLE_UNIT / 2;
}

#endif
/* CMP.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0c80_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0c90_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu060 (dsta);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0c98_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0ca0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0ca8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (6);
{	uae_s32 dst = get_long_mmu060 (dsta);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0cb0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu060 (dsta);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0cb8_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (6);
{	uae_s32 dst = get_long_mmu060 (dsta);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0cb9_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (6);
{	uae_s32 dst = get_long_mmu060 (dsta);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (10);
return 28 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cba_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 6;
	dsta += (uae_s32)(uae_s16)get_iword_mmu060 (6);
{	uae_s32 dst = get_long_mmu060 (dsta);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

#endif
/* CMP.L #<data>.L,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cbb_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (6);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 dst = get_long_mmu060 (dsta);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 24 * CYCLE_UNIT / 2;
}

#endif
/* CAS.W #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cd0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_lrmw_word_mmu060 (dsta);
	if ((dsta & 1) && currprefs.int_no_unimplemented && get_cpu_model () == 68060) {
	m68k_incpci (4);
		op_unimpl (opcode);
		goto endlabel14876;
	}
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, rc)), (uae_s16)(dst));
	if (GET_ZFLG ()){
		put_lrmw_word_mmu060 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_word_mmu060 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xffff) | (dst & 0xffff);
}}}}}}	m68k_incpci (4);
endlabel14876: ;
return 20 * CYCLE_UNIT / 2;
}

#endif
/* CAS.W #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cd8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_lrmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	if ((dsta & 1) && currprefs.int_no_unimplemented && get_cpu_model () == 68060) {
		m68k_areg (regs, dstreg) -= 2;
	m68k_incpci (4);
		op_unimpl (opcode);
		goto endlabel14877;
	}
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, rc)), (uae_s16)(dst));
	if (GET_ZFLG ()){
		put_lrmw_word_mmu060 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_word_mmu060 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xffff) | (dst & 0xffff);
}}}}}}	m68k_incpci (4);
endlabel14877: ;
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

#endif
/* CAS.W #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ce0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_lrmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	if ((dsta & 1) && currprefs.int_no_unimplemented && get_cpu_model () == 68060) {
		m68k_areg (regs, dstreg) += 2;
	m68k_incpci (4);
		op_unimpl (opcode);
		goto endlabel14878;
	}
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, rc)), (uae_s16)(dst));
	if (GET_ZFLG ()){
		put_lrmw_word_mmu060 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_word_mmu060 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xffff) | (dst & 0xffff);
}}}}}}	m68k_incpci (4);
endlabel14878: ;
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

#endif
/* CAS.W #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ce8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s16 dst = get_lrmw_word_mmu060 (dsta);
	if ((dsta & 1) && currprefs.int_no_unimplemented && get_cpu_model () == 68060) {
	m68k_incpci (6);
		op_unimpl (opcode);
		goto endlabel14879;
	}
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, rc)), (uae_s16)(dst));
	if (GET_ZFLG ()){
		put_lrmw_word_mmu060 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_word_mmu060 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xffff) | (dst & 0xffff);
}}}}}}	m68k_incpci (6);
endlabel14879: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* CAS.W #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cf0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_lrmw_word_mmu060 (dsta);
	if ((dsta & 1) && currprefs.int_no_unimplemented && get_cpu_model () == 68060) {
		op_unimpl (opcode);
		goto endlabel14880;
	}
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, rc)), (uae_s16)(dst));
	if (GET_ZFLG ()){
		put_lrmw_word_mmu060 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_word_mmu060 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xffff) | (dst & 0xffff);
}}}}}}}endlabel14880: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* CAS.W #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cf8_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s16 dst = get_lrmw_word_mmu060 (dsta);
	if ((dsta & 1) && currprefs.int_no_unimplemented && get_cpu_model () == 68060) {
	m68k_incpci (6);
		op_unimpl (opcode);
		goto endlabel14881;
	}
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, rc)), (uae_s16)(dst));
	if (GET_ZFLG ()){
		put_lrmw_word_mmu060 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_word_mmu060 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xffff) | (dst & 0xffff);
}}}}}}	m68k_incpci (6);
endlabel14881: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* CAS.W #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cf9_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_s16 dst = get_lrmw_word_mmu060 (dsta);
	if ((dsta & 1) && currprefs.int_no_unimplemented && get_cpu_model () == 68060) {
	m68k_incpci (8);
		op_unimpl (opcode);
		goto endlabel14882;
	}
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, rc)), (uae_s16)(dst));
	if (GET_ZFLG ()){
		put_lrmw_word_mmu060 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_word_mmu060 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xffff) | (dst & 0xffff);
}}}}}}	m68k_incpci (8);
endlabel14882: ;
return 28 * CYCLE_UNIT / 2;
}

#endif
/* CAS2.W #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cfc_33)(uae_u32 opcode)
{
{{	uae_s32 extra;
	extra = get_ilong_mmu060 (2);
	uae_u32 rn1 = regs.regs[(extra >> 28) & 15];
	uae_u32 rn2 = regs.regs[(extra >> 12) & 15];
	uae_u16 dst1 = get_lrmw_word_mmu060 (rn1), dst2 = get_lrmw_word_mmu060 (rn2);
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, (extra >> 16) & 7)), (uae_s16)(dst1));
	if (GET_ZFLG ()) {
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, extra & 7)), (uae_s16)(dst2));
	if (GET_ZFLG ()) {
	put_lrmw_word_mmu060 (rn1, m68k_dreg (regs, (extra >> 22) & 7));
	put_lrmw_word_mmu060 (rn2, m68k_dreg (regs, (extra >> 6) & 7));
	}}
	if (! GET_ZFLG ()) {
	m68k_dreg (regs, (extra >> 6) & 7) = (m68k_dreg (regs, (extra >> 6) & 7) & ~0xffff) | (dst2 & 0xffff);
	m68k_dreg (regs, (extra >> 22) & 7) = (m68k_dreg (regs, (extra >> 22) & 7) & ~0xffff) | (dst1 & 0xffff);
	}
}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.B #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e10_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel14884; }
{{	uae_s16 extra = get_iword_mmu060 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	dfc060_put_byte (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_s8 src = sfc060_get_byte (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
	}
}}}}}}	m68k_incpci (4);
endlabel14884: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.B #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e18_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel14885; }
{{	uae_s16 extra = get_iword_mmu060 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	dfc060_put_byte (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_s8 src = sfc060_get_byte (srca);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
	}
}}}}}}	m68k_incpci (4);
endlabel14885: ;
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.B #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e20_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel14886; }
{{	uae_s16 extra = get_iword_mmu060 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	dfc060_put_byte (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 src = sfc060_get_byte (srca);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = srca;
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
	}
}}}}}}	m68k_incpci (4);
endlabel14886: ;
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.B #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e28_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel14887; }
{{	uae_s16 extra = get_iword_mmu060 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
	dfc060_put_byte (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 src = sfc060_get_byte (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
	}
}}}}}}	m68k_incpci (6);
endlabel14887: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.B #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e30_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel14888; }
{{	uae_s16 extra = get_iword_mmu060 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	dfc060_put_byte (dsta, src);
}}}else{{	uaecptr srca;
	m68k_incpci (4);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 1);
{	uae_s8 src = sfc060_get_byte (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
	}
}}}}}}}endlabel14888: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.B #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e38_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel14889; }
{{	uae_s16 extra = get_iword_mmu060 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
	dfc060_put_byte (dsta, src);
}}else{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s8 src = sfc060_get_byte (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
	}
}}}}}}	m68k_incpci (6);
endlabel14889: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.B #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e39_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel14890; }
{{	uae_s16 extra = get_iword_mmu060 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
	dfc060_put_byte (dsta, src);
}}else{{	uaecptr srca;
	srca = get_ilong_mmu060 (4);
{	uae_s8 src = sfc060_get_byte (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
	}
}}}}}}	m68k_incpci (8);
endlabel14890: ;
return 32 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.W #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e50_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel14891; }
{{	uae_s16 extra = get_iword_mmu060 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	dfc060_put_word (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_s16 src = sfc060_get_word (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xffff) | ((src) & 0xffff);
	}
}}}}}}	m68k_incpci (4);
endlabel14891: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.W #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e58_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel14892; }
{{	uae_s16 extra = get_iword_mmu060 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	dfc060_put_word (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_s16 src = sfc060_get_word (srca);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xffff) | ((src) & 0xffff);
	}
}}}}}}	m68k_incpci (4);
endlabel14892: ;
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.W #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e60_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel14893; }
{{	uae_s16 extra = get_iword_mmu060 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	dfc060_put_word (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) - 2;
{	uae_s16 src = sfc060_get_word (srca);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = srca;
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xffff) | ((src) & 0xffff);
	}
}}}}}}	m68k_incpci (4);
endlabel14893: ;
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.W #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e68_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel14894; }
{{	uae_s16 extra = get_iword_mmu060 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
	dfc060_put_word (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s16 src = sfc060_get_word (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xffff) | ((src) & 0xffff);
	}
}}}}}}	m68k_incpci (6);
endlabel14894: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.W #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e70_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel14895; }
{{	uae_s16 extra = get_iword_mmu060 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	dfc060_put_word (dsta, src);
}}}else{{	uaecptr srca;
	m68k_incpci (4);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 1);
{	uae_s16 src = sfc060_get_word (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xffff) | ((src) & 0xffff);
	}
}}}}}}}endlabel14895: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.W #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e78_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel14896; }
{{	uae_s16 extra = get_iword_mmu060 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
	dfc060_put_word (dsta, src);
}}else{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s16 src = sfc060_get_word (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xffff) | ((src) & 0xffff);
	}
}}}}}}	m68k_incpci (6);
endlabel14896: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.W #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e79_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel14897; }
{{	uae_s16 extra = get_iword_mmu060 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
	dfc060_put_word (dsta, src);
}}else{{	uaecptr srca;
	srca = get_ilong_mmu060 (4);
{	uae_s16 src = sfc060_get_word (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xffff) | ((src) & 0xffff);
	}
}}}}}}	m68k_incpci (8);
endlabel14897: ;
return 32 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e90_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel14898; }
{{	uae_s16 extra = get_iword_mmu060 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	dfc060_put_long (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_s32 src = sfc060_get_long (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (src);
	}
}}}}}}	m68k_incpci (4);
endlabel14898: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.L #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e98_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel14899; }
{{	uae_s16 extra = get_iword_mmu060 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	dfc060_put_long (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_s32 src = sfc060_get_long (srca);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (src);
	}
}}}}}}	m68k_incpci (4);
endlabel14899: ;
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.L #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ea0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel14900; }
{{	uae_s16 extra = get_iword_mmu060 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	dfc060_put_long (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) - 4;
{	uae_s32 src = sfc060_get_long (srca);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = srca;
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (src);
	}
}}}}}}	m68k_incpci (4);
endlabel14900: ;
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ea8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel14901; }
{{	uae_s16 extra = get_iword_mmu060 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
	dfc060_put_long (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s32 src = sfc060_get_long (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (src);
	}
}}}}}}	m68k_incpci (6);
endlabel14901: ;
return 32 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0eb0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel14902; }
{{	uae_s16 extra = get_iword_mmu060 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	dfc060_put_long (dsta, src);
}}}else{{	uaecptr srca;
	m68k_incpci (4);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 1);
{	uae_s32 src = sfc060_get_long (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (src);
	}
}}}}}}}endlabel14902: ;
return 32 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0eb8_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel14903; }
{{	uae_s16 extra = get_iword_mmu060 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
	dfc060_put_long (dsta, src);
}}else{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s32 src = sfc060_get_long (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (src);
	}
}}}}}}	m68k_incpci (6);
endlabel14903: ;
return 32 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0eb9_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel14904; }
{{	uae_s16 extra = get_iword_mmu060 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
	dfc060_put_long (dsta, src);
}}else{{	uaecptr srca;
	srca = get_ilong_mmu060 (4);
{	uae_s32 src = sfc060_get_long (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (src);
	}
}}}}}}	m68k_incpci (8);
endlabel14904: ;
return 40 * CYCLE_UNIT / 2;
}

#endif
/* CAS.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ed0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_lrmw_long_mmu060 (dsta);
	if ((dsta & 3) && currprefs.int_no_unimplemented && get_cpu_model () == 68060) {
	m68k_incpci (4);
		op_unimpl (opcode);
		goto endlabel14905;
	}
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, rc)), (uae_s32)(dst));
	if (GET_ZFLG ()){
		put_lrmw_long_mmu060 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_long_mmu060 (dsta, dst);
		m68k_dreg(regs, rc) = dst;
}}}}}}	m68k_incpci (4);
endlabel14905: ;
return 32 * CYCLE_UNIT / 2;
}

#endif
/* CAS.L #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ed8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_lrmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	if ((dsta & 3) && currprefs.int_no_unimplemented && get_cpu_model () == 68060) {
		m68k_areg (regs, dstreg) -= 4;
	m68k_incpci (4);
		op_unimpl (opcode);
		goto endlabel14906;
	}
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, rc)), (uae_s32)(dst));
	if (GET_ZFLG ()){
		put_lrmw_long_mmu060 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_long_mmu060 (dsta, dst);
		m68k_dreg(regs, rc) = dst;
}}}}}}	m68k_incpci (4);
endlabel14906: ;
	mmufixup[0].reg = -1;
return 32 * CYCLE_UNIT / 2;
}

#endif
/* CAS.L #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ee0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_lrmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	if ((dsta & 3) && currprefs.int_no_unimplemented && get_cpu_model () == 68060) {
		m68k_areg (regs, dstreg) += 4;
	m68k_incpci (4);
		op_unimpl (opcode);
		goto endlabel14907;
	}
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, rc)), (uae_s32)(dst));
	if (GET_ZFLG ()){
		put_lrmw_long_mmu060 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_long_mmu060 (dsta, dst);
		m68k_dreg(regs, rc) = dst;
}}}}}}	m68k_incpci (4);
endlabel14907: ;
	mmufixup[0].reg = -1;
return 34 * CYCLE_UNIT / 2;
}

#endif
/* CAS.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ee8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s32 dst = get_lrmw_long_mmu060 (dsta);
	if ((dsta & 3) && currprefs.int_no_unimplemented && get_cpu_model () == 68060) {
	m68k_incpci (6);
		op_unimpl (opcode);
		goto endlabel14908;
	}
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, rc)), (uae_s32)(dst));
	if (GET_ZFLG ()){
		put_lrmw_long_mmu060 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_long_mmu060 (dsta, dst);
		m68k_dreg(regs, rc) = dst;
}}}}}}	m68k_incpci (6);
endlabel14908: ;
return 36 * CYCLE_UNIT / 2;
}

#endif
/* CAS.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ef0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_lrmw_long_mmu060 (dsta);
	if ((dsta & 3) && currprefs.int_no_unimplemented && get_cpu_model () == 68060) {
		op_unimpl (opcode);
		goto endlabel14909;
	}
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, rc)), (uae_s32)(dst));
	if (GET_ZFLG ()){
		put_lrmw_long_mmu060 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_long_mmu060 (dsta, dst);
		m68k_dreg(regs, rc) = dst;
}}}}}}}endlabel14909: ;
return 36 * CYCLE_UNIT / 2;
}

#endif
/* CAS.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ef8_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s32 dst = get_lrmw_long_mmu060 (dsta);
	if ((dsta & 3) && currprefs.int_no_unimplemented && get_cpu_model () == 68060) {
	m68k_incpci (6);
		op_unimpl (opcode);
		goto endlabel14910;
	}
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, rc)), (uae_s32)(dst));
	if (GET_ZFLG ()){
		put_lrmw_long_mmu060 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_long_mmu060 (dsta, dst);
		m68k_dreg(regs, rc) = dst;
}}}}}}	m68k_incpci (6);
endlabel14910: ;
return 36 * CYCLE_UNIT / 2;
}

#endif
/* CAS.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ef9_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_s32 dst = get_lrmw_long_mmu060 (dsta);
	if ((dsta & 3) && currprefs.int_no_unimplemented && get_cpu_model () == 68060) {
	m68k_incpci (8);
		op_unimpl (opcode);
		goto endlabel14911;
	}
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, rc)), (uae_s32)(dst));
	if (GET_ZFLG ()){
		put_lrmw_long_mmu060 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_long_mmu060 (dsta, dst);
		m68k_dreg(regs, rc) = dst;
}}}}}}	m68k_incpci (8);
endlabel14911: ;
return 40 * CYCLE_UNIT / 2;
}

#endif
/* CAS2.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0efc_33)(uae_u32 opcode)
{
{{	uae_s32 extra;
	extra = get_ilong_mmu060 (2);
	uae_u32 rn1 = regs.regs[(extra >> 28) & 15];
	uae_u32 rn2 = regs.regs[(extra >> 12) & 15];
	uae_u32 dst1 = get_lrmw_long_mmu060 (rn1), dst2 = get_lrmw_long_mmu060 (rn2);
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, (extra >> 16) & 7)), (uae_s32)(dst1));
	if (GET_ZFLG ()) {
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, extra & 7)), (uae_s32)(dst2));
	if (GET_ZFLG ()) {
	put_lrmw_long_mmu060 (rn1, m68k_dreg (regs, (extra >> 22) & 7));
	put_lrmw_long_mmu060 (rn2, m68k_dreg (regs, (extra >> 6) & 7));
	}}
	if (! GET_ZFLG ()) {
	m68k_dreg (regs, (extra >> 6) & 7) = dst2;
	m68k_dreg (regs, (extra >> 22) & 7) = dst1;
	}
}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* MOVE.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1000_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1010_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (2);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1018_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1020_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1028_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1030_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1038_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1039_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (6);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_103a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_103b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_103c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (4);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1080_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (2);
}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1090_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1098_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10b8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10b9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10ba_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10bb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10bc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (2);
}}}	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10f8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10f9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10fa_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10fb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10fc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1100_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (2);
}}}	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1110_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1118_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1120_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1128_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1130_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1138_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1139_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_113a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_113b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_113c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1140_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1150_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1158_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1160_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1168_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1170_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1178_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1179_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (6);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (8);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_117a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_117b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_117c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (6);
}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_1180_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_1190_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_1198_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 1);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11b8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11b9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11ba_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11bb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 1);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11bc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11f8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11f9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (6);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (8);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11fa_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11fb_33)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11fc_33)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (6);
}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (6);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (8);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13f8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (8);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13f9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (6);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (10);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13fa_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (8);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13fb_33)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13fc_33)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu060 (dsta, src);
	m68k_incpci (8);
}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2000_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* MOVE.L An,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2008_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2010_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2018_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2020_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2028_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2030_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2038_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2039_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_203a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_203b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_203c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (6);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.L Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_2040_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* MOVEA.L An,An */
uae_u32 REGPARAM2 CPUFUNC(op_2048_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* MOVEA.L (An),An */
uae_u32 REGPARAM2 CPUFUNC(op_2050_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.L (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_2058_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.L -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_2060_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* MOVEA.L (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_2068_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVEA.L (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_2070_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	m68k_areg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVEA.L (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_2078_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVEA.L (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_2079_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVEA.L (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_207a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVEA.L (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_207b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	m68k_areg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVEA.L #<data>.L,An */
uae_u32 REGPARAM2 CPUFUNC(op_207c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (6);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2080_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (2);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L An,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2088_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (2);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2090_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2098_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,An,Xn),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20b8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20b9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20ba_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20bb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20bc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (6);
}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (2);
}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L An,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (2);
}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,An,Xn),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20f8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20f9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20fa_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20fb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20fc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (6);
}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2100_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (2);
}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L An,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2108_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (2);
}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2110_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2118_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2120_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2128_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,An,Xn),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2130_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2138_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2139_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_213a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_213b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_213c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (6);
}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2140_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (4);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L An,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2148_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (4);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2150_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2158_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2160_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 26 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2168_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 28 * CYCLE_UNIT / 2;
}

#endif

#ifdef PART_3
/* MOVE.L (d8,An,Xn),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2170_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2178_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2179_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (6);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (8);
}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_217a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_217b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_217c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (6);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (8);
}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_2180_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L An,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_2188_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_2190_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_2198_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 26 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,An,Xn),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 1);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
}}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21b8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21b9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
}}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21ba_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21bb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 1);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
}}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21bc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (4);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L An,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (4);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 26 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,An,Xn),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21f8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21f9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (6);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (8);
}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21fa_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21fb_33)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21fc_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (6);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (8);
}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (6);
}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L An,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (6);
}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 30 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (8);
}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,An,Xn),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23f8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (8);
}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23f9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (6);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (10);
}}}}return 36 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23fa_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (8);
}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23fb_33)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23fc_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (6);
	optflag_testl ((uae_s32)(src));
	put_long_mmu060 (dsta, src);
	m68k_incpci (10);
}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3000_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* MOVE.W An,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3008_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3010_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (2);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3018_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3020_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3028_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3030_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3038_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3039_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (6);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_303a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_303b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_303c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (4);
}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVEA.W Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_3040_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* MOVEA.W An,An */
uae_u32 REGPARAM2 CPUFUNC(op_3048_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* MOVEA.W (An),An */
uae_u32 REGPARAM2 CPUFUNC(op_3050_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (2);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVEA.W (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_3058_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MOVEA.W -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_3060_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* MOVEA.W (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_3068_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.W (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_3070_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.W (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_3078_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.W (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_3079_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (6);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVEA.W (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_307a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.W (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_307b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.W #<data>.W,An */
uae_u32 REGPARAM2 CPUFUNC(op_307c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (4);
}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3080_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (2);
}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W An,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3088_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (2);
}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3090_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3098_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30b8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30b9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30ba_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30bb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30bc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (2);
}}}	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W An,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (2);
}}}	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30f8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30f9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30fa_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30fb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30fc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3100_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (2);
}}}	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W An,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3108_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (2);
}}}	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3110_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3118_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3120_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3128_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3130_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3138_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3139_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_313a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_313b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_313c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3140_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W An,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3148_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3150_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3158_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3160_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3168_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3170_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3178_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3179_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (6);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (8);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_317a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_317b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_317c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (6);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_3180_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W An,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_3188_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_3190_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_3198_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 1);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31b8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31b9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31ba_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31bb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 1);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31bc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W An,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31f8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31f9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (6);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (8);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31fa_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31fb_33)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (2);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31fc_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (6);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (6);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W An,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (6);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (8);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33f8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (8);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33f9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (6);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (10);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33fa_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (8);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33fb_33)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (4);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33fc_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu060 (dsta, src);
	m68k_incpci (8);
}}}return 20 * CYCLE_UNIT / 2;
}

/* NEGX.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4000_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((newv) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* NEGX.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4010_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	put_rmw_byte_mmu060 (srca, newv);
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* NEGX.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4018_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	put_rmw_byte_mmu060 (srca, newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* NEGX.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4020_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	put_rmw_byte_mmu060 (srca, newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* NEGX.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4028_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	put_rmw_byte_mmu060 (srca, newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NEGX.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4030_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	put_rmw_byte_mmu060 (srca, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NEGX.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4038_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	put_rmw_byte_mmu060 (srca, newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NEGX.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4039_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	put_rmw_byte_mmu060 (srca, newv);
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* NEGX.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4040_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | ((newv) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* NEGX.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4050_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_rmw_word_mmu060 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	put_rmw_word_mmu060 (srca, newv);
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* NEGX.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4058_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_rmw_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	put_rmw_word_mmu060 (srca, newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* NEGX.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4060_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_rmw_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	put_rmw_word_mmu060 (srca, newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* NEGX.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4068_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_rmw_word_mmu060 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	put_rmw_word_mmu060 (srca, newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NEGX.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4070_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_rmw_word_mmu060 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	put_rmw_word_mmu060 (srca, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NEGX.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4078_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_rmw_word_mmu060 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	put_rmw_word_mmu060 (srca, newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NEGX.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4079_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_rmw_word_mmu060 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	put_rmw_word_mmu060 (srca, newv);
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* NEGX.L Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4080_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	m68k_dreg (regs, srcreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* NEGX.L (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4090_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_rmw_long_mmu060 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	put_rmw_long_mmu060 (srca, newv);
}}}}}	m68k_incpci (2);
return 20 * CYCLE_UNIT / 2;
}

/* NEGX.L (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4098_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_rmw_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	put_rmw_long_mmu060 (srca, newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* NEGX.L -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_40a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_rmw_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	put_rmw_long_mmu060 (srca, newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* NEGX.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_40a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_rmw_long_mmu060 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	put_rmw_long_mmu060 (srca, newv);
}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* NEGX.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_40b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_rmw_long_mmu060 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	put_rmw_long_mmu060 (srca, newv);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* NEGX.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_40b8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_rmw_long_mmu060 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	put_rmw_long_mmu060 (srca, newv);
}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* NEGX.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_40b9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_rmw_long_mmu060 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	put_rmw_long_mmu060 (srca, newv);
}}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* MVSR2.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_40c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel15241; }
{{	MakeSR ();
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | ((regs.sr) & 0xffff);
}}}	m68k_incpci (2);
endlabel15241: ;
return 4 * CYCLE_UNIT / 2;
}

/* MVSR2.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_40d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel15242; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	MakeSR ();
	put_word_mmu060 (srca, regs.sr);
}}}	m68k_incpci (2);
endlabel15242: ;
return 8 * CYCLE_UNIT / 2;
}

/* MVSR2.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_40d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel15243; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
	MakeSR ();
	put_word_mmu060 (srca, regs.sr);
}}}	m68k_incpci (2);
endlabel15243: ;
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MVSR2.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_40e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel15244; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	MakeSR ();
	put_word_mmu060 (srca, regs.sr);
}}}	m68k_incpci (2);
endlabel15244: ;
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* MVSR2.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_40e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel15245; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	MakeSR ();
	put_word_mmu060 (srca, regs.sr);
}}}	m68k_incpci (4);
endlabel15245: ;
return 12 * CYCLE_UNIT / 2;
}

/* MVSR2.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_40f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel15246; }
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
	MakeSR ();
	put_word_mmu060 (srca, regs.sr);
}}}}endlabel15246: ;
return 12 * CYCLE_UNIT / 2;
}

/* MVSR2.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_40f8_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel15247; }
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
	MakeSR ();
	put_word_mmu060 (srca, regs.sr);
}}}	m68k_incpci (4);
endlabel15247: ;
return 12 * CYCLE_UNIT / 2;
}

/* MVSR2.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_40f9_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel15248; }
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
	MakeSR ();
	put_word_mmu060 (srca, regs.sr);
}}}	m68k_incpci (6);
endlabel15248: ;
return 16 * CYCLE_UNIT / 2;
}

/* CHK.L Dn,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4100_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15249;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15249;
	}
}}}endlabel15249: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (An),Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4110_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15250;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15250;
	}
}}}}endlabel15250: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (An)+,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4118_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15251;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15251;
	}
}}}}endlabel15251: ;
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L -(An),Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4120_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15252;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15252;
	}
}}}}endlabel15252: ;
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (d16,An),Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4128_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15253;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15253;
	}
}}}}endlabel15253: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (d8,An,Xn),Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4130_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15254;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15254;
	}
}}}}}endlabel15254: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (xxx).W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4138_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15255;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15255;
	}
}}}}endlabel15255: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (xxx).L,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4139_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (6);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15256;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15256;
	}
}}}}endlabel15256: ;
return 20 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (d16,PC),Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_413a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15257;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15257;
	}
}}}}endlabel15257: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (d8,PC,Xn),Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_413b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15258;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15258;
	}
}}}}}endlabel15258: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L #<data>.L,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_413c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (6);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15259;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15259;
	}
}}}endlabel15259: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4180_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15260;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15260;
	}
}}}endlabel15260: ;
return 4 * CYCLE_UNIT / 2;
}

/* CHK.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4190_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15261;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15261;
	}
}}}}endlabel15261: ;
return 8 * CYCLE_UNIT / 2;
}

/* CHK.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4198_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15262;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15262;
	}
}}}}endlabel15262: ;
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* CHK.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15263;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15263;
	}
}}}}endlabel15263: ;
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* CHK.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15264;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15264;
	}
}}}}endlabel15264: ;
return 12 * CYCLE_UNIT / 2;
}

/* CHK.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15265;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15265;
	}
}}}}}endlabel15265: ;
return 12 * CYCLE_UNIT / 2;
}

/* CHK.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41b8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15266;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15266;
	}
}}}}endlabel15266: ;
return 12 * CYCLE_UNIT / 2;
}

/* CHK.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41b9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (6);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15267;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15267;
	}
}}}}endlabel15267: ;
return 16 * CYCLE_UNIT / 2;
}

/* CHK.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41ba_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15268;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15268;
	}
}}}}endlabel15268: ;
return 12 * CYCLE_UNIT / 2;
}

/* CHK.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41bb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15269;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15269;
	}
}}}}}endlabel15269: ;
return 12 * CYCLE_UNIT / 2;
}

/* CHK.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41bc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel15270;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel15270;
	}
}}}endlabel15270: ;
return 8 * CYCLE_UNIT / 2;
}

/* LEA.L (An),An */
uae_u32 REGPARAM2 CPUFUNC(op_41d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	m68k_areg (regs, dstreg) = (srca);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LEA.L (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_41e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	m68k_areg (regs, dstreg) = (srca);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* LEA.L (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_41f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	m68k_areg (regs, dstreg) = (srca);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* LEA.L (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_41f8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	m68k_areg (regs, dstreg) = (srca);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* LEA.L (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_41f9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	m68k_areg (regs, dstreg) = (srca);
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* LEA.L (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_41fa_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	m68k_areg (regs, dstreg) = (srca);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* LEA.L (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_41fb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	m68k_areg (regs, dstreg) = (srca);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* CLR.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4200_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	SET_CZNV (FLAGVAL_Z);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((0) & 0xff);
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CLR.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4210_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	SET_CZNV (FLAGVAL_Z);
	put_byte_mmu060 (srca, 0);
}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* CLR.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4218_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
	SET_CZNV (FLAGVAL_Z);
	put_byte_mmu060 (srca, 0);
}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* CLR.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4220_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	SET_CZNV (FLAGVAL_Z);
	put_byte_mmu060 (srca, 0);
}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* CLR.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4228_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	SET_CZNV (FLAGVAL_Z);
	put_byte_mmu060 (srca, 0);
}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CLR.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4230_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
	SET_CZNV (FLAGVAL_Z);
	put_byte_mmu060 (srca, 0);
}}}return 12 * CYCLE_UNIT / 2;
}

/* CLR.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4238_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
	SET_CZNV (FLAGVAL_Z);
	put_byte_mmu060 (srca, 0);
}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CLR.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4239_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
	SET_CZNV (FLAGVAL_Z);
	put_byte_mmu060 (srca, 0);
}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* CLR.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4240_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	SET_CZNV (FLAGVAL_Z);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | ((0) & 0xffff);
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CLR.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4250_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	SET_CZNV (FLAGVAL_Z);
	put_word_mmu060 (srca, 0);
}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* CLR.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4258_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
	SET_CZNV (FLAGVAL_Z);
	put_word_mmu060 (srca, 0);
}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* CLR.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4260_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	SET_CZNV (FLAGVAL_Z);
	put_word_mmu060 (srca, 0);
}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* CLR.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4268_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	SET_CZNV (FLAGVAL_Z);
	put_word_mmu060 (srca, 0);
}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CLR.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4270_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
	SET_CZNV (FLAGVAL_Z);
	put_word_mmu060 (srca, 0);
}}}return 12 * CYCLE_UNIT / 2;
}

/* CLR.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4278_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
	SET_CZNV (FLAGVAL_Z);
	put_word_mmu060 (srca, 0);
}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CLR.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4279_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
	SET_CZNV (FLAGVAL_Z);
	put_word_mmu060 (srca, 0);
}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* CLR.L Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4280_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	SET_CZNV (FLAGVAL_Z);
	m68k_dreg (regs, srcreg) = (0);
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CLR.L (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4290_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	SET_CZNV (FLAGVAL_Z);
	put_long_mmu060 (srca, 0);
}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* CLR.L (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4298_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
	SET_CZNV (FLAGVAL_Z);
	put_long_mmu060 (srca, 0);
}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* CLR.L -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_42a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	SET_CZNV (FLAGVAL_Z);
	put_long_mmu060 (srca, 0);
}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* CLR.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_42a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	SET_CZNV (FLAGVAL_Z);
	put_long_mmu060 (srca, 0);
}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CLR.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_42b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
	SET_CZNV (FLAGVAL_Z);
	put_long_mmu060 (srca, 0);
}}}return 16 * CYCLE_UNIT / 2;
}

/* CLR.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_42b8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
	SET_CZNV (FLAGVAL_Z);
	put_long_mmu060 (srca, 0);
}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CLR.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_42b9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
	SET_CZNV (FLAGVAL_Z);
	put_long_mmu060 (srca, 0);
}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* MVSR2.B Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	MakeSR ();
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | ((regs.sr & 0xff) & 0xffff);
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

#endif
#endif

#ifdef PART_4
/* MVSR2.B (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	MakeSR ();
	put_word_mmu060 (srca, regs.sr & 0xff);
}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* MVSR2.B (An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
	MakeSR ();
	put_word_mmu060 (srca, regs.sr & 0xff);
}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* MVSR2.B -(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	MakeSR ();
	put_word_mmu060 (srca, regs.sr & 0xff);
}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

#endif
/* MVSR2.B (d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	MakeSR ();
	put_word_mmu060 (srca, regs.sr & 0xff);
}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* MVSR2.B (d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
	MakeSR ();
	put_word_mmu060 (srca, regs.sr & 0xff);
}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* MVSR2.B (xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42f8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
	MakeSR ();
	put_word_mmu060 (srca, regs.sr & 0xff);
}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* MVSR2.B (xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42f9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
	MakeSR ();
	put_word_mmu060 (srca, regs.sr & 0xff);
}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* NEG.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4400_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((dst) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* NEG.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4410_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	put_rmw_byte_mmu060 (srca, dst);
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* NEG.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4418_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	put_rmw_byte_mmu060 (srca, dst);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* NEG.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4420_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	put_rmw_byte_mmu060 (srca, dst);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* NEG.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4428_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	put_rmw_byte_mmu060 (srca, dst);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NEG.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4430_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	put_rmw_byte_mmu060 (srca, dst);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NEG.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4438_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	put_rmw_byte_mmu060 (srca, dst);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NEG.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4439_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	put_rmw_byte_mmu060 (srca, dst);
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* NEG.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4440_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | ((dst) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* NEG.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4450_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_rmw_word_mmu060 (srca);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	put_rmw_word_mmu060 (srca, dst);
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* NEG.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4458_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_rmw_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	put_rmw_word_mmu060 (srca, dst);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* NEG.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4460_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_rmw_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	put_rmw_word_mmu060 (srca, dst);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* NEG.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4468_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_rmw_word_mmu060 (srca);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	put_rmw_word_mmu060 (srca, dst);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NEG.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4470_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_rmw_word_mmu060 (srca);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	put_rmw_word_mmu060 (srca, dst);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NEG.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4478_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_rmw_word_mmu060 (srca);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	put_rmw_word_mmu060 (srca, dst);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NEG.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4479_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_rmw_word_mmu060 (srca);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	put_rmw_word_mmu060 (srca, dst);
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* NEG.L Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4480_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	m68k_dreg (regs, srcreg) = (dst);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* NEG.L (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4490_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_rmw_long_mmu060 (srca);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	put_rmw_long_mmu060 (srca, dst);
}}}}}	m68k_incpci (2);
return 20 * CYCLE_UNIT / 2;
}

/* NEG.L (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4498_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_rmw_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	put_rmw_long_mmu060 (srca, dst);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* NEG.L -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_44a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_rmw_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	put_rmw_long_mmu060 (srca, dst);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* NEG.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_44a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_rmw_long_mmu060 (srca);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	put_rmw_long_mmu060 (srca, dst);
}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* NEG.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_44b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_rmw_long_mmu060 (srca);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	put_rmw_long_mmu060 (srca, dst);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* NEG.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_44b8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_rmw_long_mmu060 (srca);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	put_rmw_long_mmu060 (srca, dst);
}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* NEG.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_44b9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_rmw_long_mmu060 (srca);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	put_rmw_long_mmu060 (srca, dst);
}}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* MV2SR.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_44c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* MV2SR.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_44d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* MV2SR.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_44d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MV2SR.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_44e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* MV2SR.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_44e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_44f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_44f8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_44f9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* MV2SR.B (d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_44fa_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.B (d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_44fb_33)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.B #<data>.B */
uae_u32 REGPARAM2 CPUFUNC(op_44fc_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* NOT.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4600_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((dst) & 0xff);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* NOT.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4610_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	put_rmw_byte_mmu060 (srca, dst);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* NOT.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4618_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	put_rmw_byte_mmu060 (srca, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* NOT.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4620_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	put_rmw_byte_mmu060 (srca, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* NOT.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4628_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	put_rmw_byte_mmu060 (srca, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NOT.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4630_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	put_rmw_byte_mmu060 (srca, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NOT.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4638_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	put_rmw_byte_mmu060 (srca, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NOT.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4639_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	put_rmw_byte_mmu060 (srca, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* NOT.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4640_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | ((dst) & 0xffff);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* NOT.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4650_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_rmw_word_mmu060 (srca);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	put_rmw_word_mmu060 (srca, dst);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* NOT.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4658_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_rmw_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	put_rmw_word_mmu060 (srca, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* NOT.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4660_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_rmw_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	put_rmw_word_mmu060 (srca, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* NOT.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4668_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_rmw_word_mmu060 (srca);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	put_rmw_word_mmu060 (srca, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NOT.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4670_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_rmw_word_mmu060 (srca);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	put_rmw_word_mmu060 (srca, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NOT.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4678_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_rmw_word_mmu060 (srca);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	put_rmw_word_mmu060 (srca, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NOT.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4679_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_rmw_word_mmu060 (srca);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	put_rmw_word_mmu060 (srca, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* NOT.L Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4680_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	m68k_dreg (regs, srcreg) = (dst);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* NOT.L (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4690_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_rmw_long_mmu060 (srca);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	put_rmw_long_mmu060 (srca, dst);
}}}}	m68k_incpci (2);
return 20 * CYCLE_UNIT / 2;
}

/* NOT.L (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4698_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_rmw_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	put_rmw_long_mmu060 (srca, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* NOT.L -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_46a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_rmw_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	put_rmw_long_mmu060 (srca, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* NOT.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_46a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_rmw_long_mmu060 (srca);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	put_rmw_long_mmu060 (srca, dst);
}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* NOT.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_46b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_rmw_long_mmu060 (srca);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	put_rmw_long_mmu060 (srca, dst);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* NOT.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_46b8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_rmw_long_mmu060 (srca);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	put_rmw_long_mmu060 (srca, dst);
}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* NOT.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_46b9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_rmw_long_mmu060 (srca);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	put_rmw_long_mmu060 (srca, dst);
}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* MV2SR.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_46c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel15369; }
{{	uae_s16 src = m68k_dreg (regs, srcreg);
	regs.sr = src;
	MakeFromSR ();
}}}	m68k_incpci (2);
endlabel15369: ;
return 4 * CYCLE_UNIT / 2;
}

/* MV2SR.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_46d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel15370; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	regs.sr = src;
	MakeFromSR ();
}}}}	m68k_incpci (2);
endlabel15370: ;
return 8 * CYCLE_UNIT / 2;
}

/* MV2SR.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_46d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel15371; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
	regs.sr = src;
	MakeFromSR ();
}}}}	m68k_incpci (2);
endlabel15371: ;
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MV2SR.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_46e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel15372; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	regs.sr = src;
	MakeFromSR ();
}}}}	m68k_incpci (2);
endlabel15372: ;
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* MV2SR.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_46e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel15373; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
	regs.sr = src;
	MakeFromSR ();
}}}}	m68k_incpci (4);
endlabel15373: ;
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_46f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel15374; }
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
	regs.sr = src;
	MakeFromSR ();
}}}}}endlabel15374: ;
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_46f8_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel15375; }
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
	regs.sr = src;
	MakeFromSR ();
}}}}	m68k_incpci (4);
endlabel15375: ;
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_46f9_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel15376; }
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
	regs.sr = src;
	MakeFromSR ();
}}}}	m68k_incpci (6);
endlabel15376: ;
return 16 * CYCLE_UNIT / 2;
}

/* MV2SR.W (d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_46fa_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel15377; }
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
	regs.sr = src;
	MakeFromSR ();
}}}}	m68k_incpci (4);
endlabel15377: ;
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.W (d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_46fb_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel15378; }
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
	regs.sr = src;
	MakeFromSR ();
}}}}}endlabel15378: ;
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_46fc_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel15379; }
{{	uae_s16 src = get_iword_mmu060 (2);
	regs.sr = src;
	MakeFromSR ();
}}}	m68k_incpci (4);
endlabel15379: ;
return 8 * CYCLE_UNIT / 2;
}

/* NBCD.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4800_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_u16 newv_lo = - (src & 0xF) - (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	int cflg, tmp_newv;
	if (newv_lo > 9) { newv_lo -= 6; }
	tmp_newv = newv = newv_hi + newv_lo;
	cflg = (newv & 0x1F0) > 0x90;
	if (cflg) newv -= 0x60;
	SET_CFLG (cflg);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((newv) & 0xff);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LINK.L An,#<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4808_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
{	uae_s32 offs;
	offs = get_ilong_mmu060 (2);
{	uaecptr olda;
	olda = m68k_areg (regs, 7) - 4;
	mmufixup[1].reg = 7;
	mmufixup[1].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = olda;
{	uae_s32 src = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = (m68k_areg(regs, 7));
	m68k_areg(regs, 7) += offs;
	put_long_mmu060 (olda, src);
}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

#endif
/* NBCD.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4810_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
{	uae_u16 newv_lo = - (src & 0xF) - (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	int cflg, tmp_newv;
	if (newv_lo > 9) { newv_lo -= 6; }
	tmp_newv = newv = newv_hi + newv_lo;
	cflg = (newv & 0x1F0) > 0x90;
	if (cflg) newv -= 0x60;
	SET_CFLG (cflg);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	put_rmw_byte_mmu060 (srca, newv);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* NBCD.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4818_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_u16 newv_lo = - (src & 0xF) - (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	int cflg, tmp_newv;
	if (newv_lo > 9) { newv_lo -= 6; }
	tmp_newv = newv = newv_hi + newv_lo;
	cflg = (newv & 0x1F0) > 0x90;
	if (cflg) newv -= 0x60;
	SET_CFLG (cflg);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	put_rmw_byte_mmu060 (srca, newv);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* NBCD.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4820_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_u16 newv_lo = - (src & 0xF) - (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	int cflg, tmp_newv;
	if (newv_lo > 9) { newv_lo -= 6; }
	tmp_newv = newv = newv_hi + newv_lo;
	cflg = (newv & 0x1F0) > 0x90;
	if (cflg) newv -= 0x60;
	SET_CFLG (cflg);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	put_rmw_byte_mmu060 (srca, newv);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* NBCD.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4828_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
{	uae_u16 newv_lo = - (src & 0xF) - (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	int cflg, tmp_newv;
	if (newv_lo > 9) { newv_lo -= 6; }
	tmp_newv = newv = newv_hi + newv_lo;
	cflg = (newv & 0x1F0) > 0x90;
	if (cflg) newv -= 0x60;
	SET_CFLG (cflg);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	put_rmw_byte_mmu060 (srca, newv);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NBCD.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4830_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
{	uae_u16 newv_lo = - (src & 0xF) - (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	int cflg, tmp_newv;
	if (newv_lo > 9) { newv_lo -= 6; }
	tmp_newv = newv = newv_hi + newv_lo;
	cflg = (newv & 0x1F0) > 0x90;
	if (cflg) newv -= 0x60;
	SET_CFLG (cflg);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	put_rmw_byte_mmu060 (srca, newv);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NBCD.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4838_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
{	uae_u16 newv_lo = - (src & 0xF) - (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	int cflg, tmp_newv;
	if (newv_lo > 9) { newv_lo -= 6; }
	tmp_newv = newv = newv_hi + newv_lo;
	cflg = (newv & 0x1F0) > 0x90;
	if (cflg) newv -= 0x60;
	SET_CFLG (cflg);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	put_rmw_byte_mmu060 (srca, newv);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NBCD.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4839_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s8 src = get_rmw_byte_mmu060 (srca);
{	uae_u16 newv_lo = - (src & 0xF) - (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	int cflg, tmp_newv;
	if (newv_lo > 9) { newv_lo -= 6; }
	tmp_newv = newv = newv_hi + newv_lo;
	cflg = (newv & 0x1F0) > 0x90;
	if (cflg) newv -= 0x60;
	SET_CFLG (cflg);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	put_rmw_byte_mmu060 (srca, newv);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* SWAP.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4840_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_u32 dst = ((src >> 16)&0xFFFF) | ((src&0xFFFF)<<16);
	optflag_testl ((uae_s32)(dst));
	m68k_dreg (regs, srcreg) = (dst);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* BKPTQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4848_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* PEA.L (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4850_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = dsta;
	put_long_mmu060 (dsta, srca);
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* PEA.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4868_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = dsta;
	put_long_mmu060 (dsta, srca);
}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* PEA.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4870_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = dsta;
	put_long_mmu060 (dsta, srca);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* PEA.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4878_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = dsta;
	put_long_mmu060 (dsta, srca);
}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* PEA.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4879_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = dsta;
	put_long_mmu060 (dsta, srca);
}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* PEA.L (d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_487a_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = dsta;
	put_long_mmu060 (dsta, srca);
}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* PEA.L (d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_487b_33)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = dsta;
	put_long_mmu060 (dsta, srca);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* EXT.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4880_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_u16 dst = (uae_s16)(uae_s8)src;
	optflag_testw ((uae_s16)(dst));
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | ((dst) & 0xffff);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* MVMLE.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4890_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu060 (2);
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) {
		put_word_mmu060 (srca, m68k_dreg (regs, movem_index1[dmask]));
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		put_word_mmu060 (srca, m68k_areg (regs, movem_index1[amask]));
		srca += 2;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* MVMLE.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_48a0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu060 (2);
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) - 0;
{	uae_u16 amask = mask & 0xff, dmask = (mask >> 8) & 0xff;
	while (amask) {
		srca -= 2;
		put_word_mmu060 (srca, m68k_areg (regs, movem_index2[amask]));
		amask = movem_next[amask];
	}
	while (dmask) {
		srca -= 2;
		put_word_mmu060 (srca, m68k_dreg (regs, movem_index2[dmask]));
		dmask = movem_next[dmask];
	}
	m68k_areg (regs, dstreg) = srca;
}}}	m68k_incpci (4);
return 10 * CYCLE_UNIT / 2;
}

/* MVMLE.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_48a8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu060 (2);
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) {
		put_word_mmu060 (srca, m68k_dreg (regs, movem_index1[dmask]));
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		put_word_mmu060 (srca, m68k_areg (regs, movem_index1[amask]));
		srca += 2;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMLE.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_48b0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu060 (2);
{	uaecptr srca;
	m68k_incpci (4);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) {
		put_word_mmu060 (srca, m68k_dreg (regs, movem_index1[dmask]));
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		put_word_mmu060 (srca, m68k_areg (regs, movem_index1[amask]));
		srca += 2;
		amask = movem_next[amask];
	}
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MVMLE.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_48b8_33)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu060 (2);
{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) {
		put_word_mmu060 (srca, m68k_dreg (regs, movem_index1[dmask]));
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		put_word_mmu060 (srca, m68k_areg (regs, movem_index1[amask]));
		srca += 2;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMLE.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_48b9_33)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu060 (2);
{	uaecptr srca;
	srca = get_ilong_mmu060 (4);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) {
		put_word_mmu060 (srca, m68k_dreg (regs, movem_index1[dmask]));
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		put_word_mmu060 (srca, m68k_areg (regs, movem_index1[amask]));
		srca += 2;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

/* EXT.L Dn */
uae_u32 REGPARAM2 CPUFUNC(op_48c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_u32 dst = (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(dst));
	m68k_dreg (regs, srcreg) = (dst);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* MVMLE.L #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_48d0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu060 (2);
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) {
		put_long_mmu060 (srca, m68k_dreg (regs, movem_index1[dmask]));
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		put_long_mmu060 (srca, m68k_areg (regs, movem_index1[amask]));
		srca += 4;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* MVMLE.L #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_48e0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu060 (2);
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) - 0;
{	uae_u16 amask = mask & 0xff, dmask = (mask >> 8) & 0xff;
	while (amask) {
		srca -= 4;
		put_long_mmu060 (srca, m68k_areg (regs, movem_index2[amask]));
		amask = movem_next[amask];
	}
	while (dmask) {
		srca -= 4;
		put_long_mmu060 (srca, m68k_dreg (regs, movem_index2[dmask]));
		dmask = movem_next[dmask];
	}
	m68k_areg (regs, dstreg) = srca;
}}}	m68k_incpci (4);
return 10 * CYCLE_UNIT / 2;
}

/* MVMLE.L #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_48e8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu060 (2);
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) {
		put_long_mmu060 (srca, m68k_dreg (regs, movem_index1[dmask]));
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		put_long_mmu060 (srca, m68k_areg (regs, movem_index1[amask]));
		srca += 4;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMLE.L #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_48f0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu060 (2);
{	uaecptr srca;
	m68k_incpci (4);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) {
		put_long_mmu060 (srca, m68k_dreg (regs, movem_index1[dmask]));
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		put_long_mmu060 (srca, m68k_areg (regs, movem_index1[amask]));
		srca += 4;
		amask = movem_next[amask];
	}
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MVMLE.L #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_48f8_33)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu060 (2);
{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) {
		put_long_mmu060 (srca, m68k_dreg (regs, movem_index1[dmask]));
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		put_long_mmu060 (srca, m68k_areg (regs, movem_index1[amask]));
		srca += 4;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMLE.L #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_48f9_33)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu060 (2);
{	uaecptr srca;
	srca = get_ilong_mmu060 (4);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) {
		put_long_mmu060 (srca, m68k_dreg (regs, movem_index1[dmask]));
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		put_long_mmu060 (srca, m68k_areg (regs, movem_index1[amask]));
		srca += 4;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

/* EXT.B Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_49c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_u32 dst = (uae_s32)(uae_s8)src;
	optflag_testl ((uae_s32)(dst));
	m68k_dreg (regs, srcreg) = (dst);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

#endif
/* TST.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4a00_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
	optflag_testb ((uae_s8)(src));
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* TST.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4a10_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* TST.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4a18_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* TST.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4a20_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* TST.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4a28_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* TST.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4a30_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu060 (srca);
	optflag_testb ((uae_s8)(src));
}}}}return 12 * CYCLE_UNIT / 2;
}

/* TST.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4a38_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* TST.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4a39_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TST.B (d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4a3a_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TST.B (d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4a3b_33)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu060 (srca);
	optflag_testb ((uae_s8)(src));
}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* TST.B #<data>.B */
uae_u32 REGPARAM2 CPUFUNC(op_4a3c_33)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu060 (2);
	optflag_testb ((uae_s8)(src));
}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* TST.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4a40_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
	optflag_testw ((uae_s16)(src));
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* TST.W An */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4a48_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_areg (regs, srcreg);
	optflag_testw ((uae_s16)(src));
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

#endif
/* TST.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4a50_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	optflag_testw ((uae_s16)(src));
}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* TST.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4a58_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
	optflag_testw ((uae_s16)(src));
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* TST.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4a60_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	optflag_testw ((uae_s16)(src));
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* TST.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4a68_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
	optflag_testw ((uae_s16)(src));
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* TST.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4a70_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
	optflag_testw ((uae_s16)(src));
}}}}return 12 * CYCLE_UNIT / 2;
}

/* TST.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4a78_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
	optflag_testw ((uae_s16)(src));
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* TST.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4a79_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
	optflag_testw ((uae_s16)(src));
}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TST.W (d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4a7a_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
	optflag_testw ((uae_s16)(src));
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TST.W (d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4a7b_33)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
	optflag_testw ((uae_s16)(src));
}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* TST.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_4a7c_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
	optflag_testw ((uae_s16)(src));
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* TST.L Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4a80_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
	optflag_testl ((uae_s32)(src));
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* TST.L An */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4a88_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_areg (regs, srcreg);
	optflag_testl ((uae_s32)(src));
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

#endif
/* TST.L (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4a90_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
	optflag_testl ((uae_s32)(src));
}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* TST.L (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4a98_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
	optflag_testl ((uae_s32)(src));
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* TST.L -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4aa0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	optflag_testl ((uae_s32)(src));
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* TST.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4aa8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
	optflag_testl ((uae_s32)(src));
}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* TST.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4ab0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu060 (srca);
	optflag_testl ((uae_s32)(src));
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TST.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4ab8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
	optflag_testl ((uae_s32)(src));
}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* TST.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4ab9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
	optflag_testl ((uae_s32)(src));
}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* TST.L (d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4aba_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
	optflag_testl ((uae_s32)(src));
}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* TST.L (d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4abb_33)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu060 (srca);
	optflag_testl ((uae_s32)(src));
}}}}return 16 * CYCLE_UNIT / 2;
}

#endif
/* TST.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_4abc_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
	optflag_testl ((uae_s32)(src));
}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* TAS.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4ac0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((src) & 0xff);
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* TAS.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ad0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_lrmw_byte_mmu060 (srca);
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	put_lrmw_byte_mmu060 (srca, src);
}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* TAS.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4ad8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_lrmw_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	put_lrmw_byte_mmu060 (srca, src);
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* TAS.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ae0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_lrmw_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	put_lrmw_byte_mmu060 (srca, src);
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* TAS.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ae8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_lrmw_byte_mmu060 (srca);
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	put_lrmw_byte_mmu060 (srca, src);
}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* TAS.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4af0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_lrmw_byte_mmu060 (srca);
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	put_lrmw_byte_mmu060 (srca, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TAS.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4af8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_lrmw_byte_mmu060 (srca);
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	put_lrmw_byte_mmu060 (srca, src);
}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* TAS.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4af9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s8 src = get_lrmw_byte_mmu060 (srca);
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	put_lrmw_byte_mmu060 (srca, src);
}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* MULL.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c00_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	m68k_mull(opcode, dst, extra);
}}}return 8 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c10_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu060 (dsta);
	m68k_incpci (4);
	m68k_mull(opcode, dst, extra);
}}}}return 16 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c18_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	m68k_incpci (4);
	m68k_mull(opcode, dst, extra);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c20_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	m68k_incpci (4);
	m68k_mull(opcode, dst, extra);
}}}}	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c28_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s32 dst = get_long_mmu060 (dsta);
	m68k_incpci (6);
	m68k_mull(opcode, dst, extra);
}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c30_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu060 (dsta);
	m68k_mull(opcode, dst, extra);
}}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c38_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s32 dst = get_long_mmu060 (dsta);
	m68k_incpci (6);
	m68k_mull(opcode, dst, extra);
}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c39_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_s32 dst = get_long_mmu060 (dsta);
	m68k_incpci (8);
	m68k_mull(opcode, dst, extra);
}}}}return 24 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c3a_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s32 dst = get_long_mmu060 (dsta);
	m68k_incpci (6);
	m68k_mull(opcode, dst, extra);
}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c3b_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 dst = get_long_mmu060 (dsta);
	m68k_mull(opcode, dst, extra);
}}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,#<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c3c_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uae_s32 dst;
	dst = get_ilong_mmu060 (4);
	m68k_incpci (8);
	m68k_mull(opcode, dst, extra);
}}}return 16 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c40_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	m68k_divl(opcode, dst, extra);
}}}return 8 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c50_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu060 (dsta);
	m68k_incpci (4);
	m68k_divl(opcode, dst, extra);
}}}}return 16 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c58_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	m68k_incpci (4);
	m68k_divl(opcode, dst, extra);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c60_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	m68k_incpci (4);
	m68k_divl(opcode, dst, extra);
}}}}	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c68_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s32 dst = get_long_mmu060 (dsta);
	m68k_incpci (6);
	m68k_divl(opcode, dst, extra);
}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c70_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu060 (dsta);
	m68k_divl(opcode, dst, extra);
}}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c78_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s32 dst = get_long_mmu060 (dsta);
	m68k_incpci (6);
	m68k_divl(opcode, dst, extra);
}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c79_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_s32 dst = get_long_mmu060 (dsta);
	m68k_incpci (8);
	m68k_divl(opcode, dst, extra);
}}}}return 24 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c7a_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_s32 dst = get_long_mmu060 (dsta);
	m68k_incpci (6);
	m68k_divl(opcode, dst, extra);
}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c7b_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 dst = get_long_mmu060 (dsta);
	m68k_divl(opcode, dst, extra);
}}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,#<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c7c_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uae_s32 dst;
	dst = get_ilong_mmu060 (4);
	m68k_incpci (8);
	m68k_divl(opcode, dst, extra);
}}}return 16 * CYCLE_UNIT / 2;
}

#endif
/* MVMEL.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4c90_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu060 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_u32 tmp[16];
	int tmpreg[16];
	int idx = 0;
	while (dmask) {
		tmpreg[idx] = movem_index1[dmask] + 0;
		tmp[idx++] = (uae_s32)(uae_s16)get_word_mmu060 (srca);
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		tmpreg[idx] = movem_index1[amask] + 8;
		tmp[idx++] = (uae_s32)(uae_s16)get_word_mmu060 (srca);
		srca += 2;
		amask = movem_next[amask];
	}
	while (--idx >= 0) {
		regs.regs[tmpreg[idx]] = tmp[idx];
	}
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* MVMEL.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4c98_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu060 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_u32 tmp[16];
	int tmpreg[16];
	int idx = 0;
	while (dmask) {
		tmpreg[idx] = movem_index1[dmask] + 0;
		tmp[idx++] = (uae_s32)(uae_s16)get_word_mmu060 (srca);
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		tmpreg[idx] = movem_index1[amask] + 8;
		tmp[idx++] = (uae_s32)(uae_s16)get_word_mmu060 (srca);
		srca += 2;
		amask = movem_next[amask];
	}
	m68k_areg (regs, dstreg) = srca;
	while (--idx >= 0) {
		regs.regs[tmpreg[idx]] = tmp[idx];
	}
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* MVMEL.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ca8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu060 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 tmp[16];
	int tmpreg[16];
	int idx = 0;
	while (dmask) {
		tmpreg[idx] = movem_index1[dmask] + 0;
		tmp[idx++] = (uae_s32)(uae_s16)get_word_mmu060 (srca);
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		tmpreg[idx] = movem_index1[amask] + 8;
		tmp[idx++] = (uae_s32)(uae_s16)get_word_mmu060 (srca);
		srca += 2;
		amask = movem_next[amask];
	}
	while (--idx >= 0) {
		regs.regs[tmpreg[idx]] = tmp[idx];
	}
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4cb0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu060 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	m68k_incpci (4);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_u32 tmp[16];
	int tmpreg[16];
	int idx = 0;
	while (dmask) {
		tmpreg[idx] = movem_index1[dmask] + 0;
		tmp[idx++] = (uae_s32)(uae_s16)get_word_mmu060 (srca);
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		tmpreg[idx] = movem_index1[amask] + 8;
		tmp[idx++] = (uae_s32)(uae_s16)get_word_mmu060 (srca);
		srca += 2;
		amask = movem_next[amask];
	}
	while (--idx >= 0) {
		regs.regs[tmpreg[idx]] = tmp[idx];
	}
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4cb8_33)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu060 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 tmp[16];
	int tmpreg[16];
	int idx = 0;
	while (dmask) {
		tmpreg[idx] = movem_index1[dmask] + 0;
		tmp[idx++] = (uae_s32)(uae_s16)get_word_mmu060 (srca);
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		tmpreg[idx] = movem_index1[amask] + 8;
		tmp[idx++] = (uae_s32)(uae_s16)get_word_mmu060 (srca);
		srca += 2;
		amask = movem_next[amask];
	}
	while (--idx >= 0) {
		regs.regs[tmpreg[idx]] = tmp[idx];
	}
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4cb9_33)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu060 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = get_ilong_mmu060 (4);
{	uae_u32 tmp[16];
	int tmpreg[16];
	int idx = 0;
	while (dmask) {
		tmpreg[idx] = movem_index1[dmask] + 0;
		tmp[idx++] = (uae_s32)(uae_s16)get_word_mmu060 (srca);
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		tmpreg[idx] = movem_index1[amask] + 8;
		tmp[idx++] = (uae_s32)(uae_s16)get_word_mmu060 (srca);
		srca += 2;
		amask = movem_next[amask];
	}
	while (--idx >= 0) {
		regs.regs[tmpreg[idx]] = tmp[idx];
	}
}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

/* MVMEL.W #<data>.W,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_4cba_33)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu060 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = m68k_getpc () + 4;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 tmp[16];
	int tmpreg[16];
	int idx = 0;
	while (dmask) {
		tmpreg[idx] = movem_index1[dmask] + 0;
		tmp[idx++] = (uae_s32)(uae_s16)get_word_mmu060 (srca);
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		tmpreg[idx] = movem_index1[amask] + 8;
		tmp[idx++] = (uae_s32)(uae_s16)get_word_mmu060 (srca);
		srca += 2;
		amask = movem_next[amask];
	}
	while (--idx >= 0) {
		regs.regs[tmpreg[idx]] = tmp[idx];
	}
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.W #<data>.W,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4cbb_33)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu060 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_u32 tmp[16];
	int tmpreg[16];
	int idx = 0;
	while (dmask) {
		tmpreg[idx] = movem_index1[dmask] + 0;
		tmp[idx++] = (uae_s32)(uae_s16)get_word_mmu060 (srca);
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		tmpreg[idx] = movem_index1[amask] + 8;
		tmp[idx++] = (uae_s32)(uae_s16)get_word_mmu060 (srca);
		srca += 2;
		amask = movem_next[amask];
	}
	while (--idx >= 0) {
		regs.regs[tmpreg[idx]] = tmp[idx];
	}
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4cd0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu060 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_u32 tmp[16];
	int tmpreg[16];
	int idx = 0;
	while (dmask) {
		tmpreg[idx] = movem_index1[dmask] + 0;
		tmp[idx++] = get_long_mmu060 (srca);
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		tmpreg[idx] = movem_index1[amask] + 8;
		tmp[idx++] = get_long_mmu060 (srca);
		srca += 4;
		amask = movem_next[amask];
	}
	while (--idx >= 0) {
		regs.regs[tmpreg[idx]] = tmp[idx];
	}
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4cd8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu060 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_u32 tmp[16];
	int tmpreg[16];
	int idx = 0;
	while (dmask) {
		tmpreg[idx] = movem_index1[dmask] + 0;
		tmp[idx++] = get_long_mmu060 (srca);
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		tmpreg[idx] = movem_index1[amask] + 8;
		tmp[idx++] = get_long_mmu060 (srca);
		srca += 4;
		amask = movem_next[amask];
	}
	m68k_areg (regs, dstreg) = srca;
	while (--idx >= 0) {
		regs.regs[tmpreg[idx]] = tmp[idx];
	}
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ce8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu060 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 tmp[16];
	int tmpreg[16];
	int idx = 0;
	while (dmask) {
		tmpreg[idx] = movem_index1[dmask] + 0;
		tmp[idx++] = get_long_mmu060 (srca);
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		tmpreg[idx] = movem_index1[amask] + 8;
		tmp[idx++] = get_long_mmu060 (srca);
		srca += 4;
		amask = movem_next[amask];
	}
	while (--idx >= 0) {
		regs.regs[tmpreg[idx]] = tmp[idx];
	}
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4cf0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu060 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	m68k_incpci (4);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_u32 tmp[16];
	int tmpreg[16];
	int idx = 0;
	while (dmask) {
		tmpreg[idx] = movem_index1[dmask] + 0;
		tmp[idx++] = get_long_mmu060 (srca);
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		tmpreg[idx] = movem_index1[amask] + 8;
		tmp[idx++] = get_long_mmu060 (srca);
		srca += 4;
		amask = movem_next[amask];
	}
	while (--idx >= 0) {
		regs.regs[tmpreg[idx]] = tmp[idx];
	}
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4cf8_33)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu060 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 tmp[16];
	int tmpreg[16];
	int idx = 0;
	while (dmask) {
		tmpreg[idx] = movem_index1[dmask] + 0;
		tmp[idx++] = get_long_mmu060 (srca);
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		tmpreg[idx] = movem_index1[amask] + 8;
		tmp[idx++] = get_long_mmu060 (srca);
		srca += 4;
		amask = movem_next[amask];
	}
	while (--idx >= 0) {
		regs.regs[tmpreg[idx]] = tmp[idx];
	}
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4cf9_33)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu060 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = get_ilong_mmu060 (4);
{	uae_u32 tmp[16];
	int tmpreg[16];
	int idx = 0;
	while (dmask) {
		tmpreg[idx] = movem_index1[dmask] + 0;
		tmp[idx++] = get_long_mmu060 (srca);
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		tmpreg[idx] = movem_index1[amask] + 8;
		tmp[idx++] = get_long_mmu060 (srca);
		srca += 4;
		amask = movem_next[amask];
	}
	while (--idx >= 0) {
		regs.regs[tmpreg[idx]] = tmp[idx];
	}
}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_4cfa_33)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu060 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = m68k_getpc () + 4;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 tmp[16];
	int tmpreg[16];
	int idx = 0;
	while (dmask) {
		tmpreg[idx] = movem_index1[dmask] + 0;
		tmp[idx++] = get_long_mmu060 (srca);
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		tmpreg[idx] = movem_index1[amask] + 8;
		tmp[idx++] = get_long_mmu060 (srca);
		srca += 4;
		amask = movem_next[amask];
	}
	while (--idx >= 0) {
		regs.regs[tmpreg[idx]] = tmp[idx];
	}
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4cfb_33)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu060 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_u32 tmp[16];
	int tmpreg[16];
	int idx = 0;
	while (dmask) {
		tmpreg[idx] = movem_index1[dmask] + 0;
		tmp[idx++] = get_long_mmu060 (srca);
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		tmpreg[idx] = movem_index1[amask] + 8;
		tmp[idx++] = get_long_mmu060 (srca);
		srca += 4;
		amask = movem_next[amask];
	}
	while (--idx >= 0) {
		regs.regs[tmpreg[idx]] = tmp[idx];
	}
}}}}return 12 * CYCLE_UNIT / 2;
}

/* TRAPQ.L #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_4e40_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 15);
{{	uae_u32 src = srcreg;
	m68k_incpci (2);
	Exception (src + 32);
}}return 4 * CYCLE_UNIT / 2;
}

/* LINK.W An,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_4e50_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu060 (2);
{	uaecptr olda;
	olda = m68k_areg (regs, 7) - 4;
	mmufixup[1].reg = 7;
	mmufixup[1].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = olda;
{	uae_s32 src = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = (m68k_areg(regs, 7));
	m68k_areg(regs, 7) += offs;
	put_long_mmu060 (olda, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* UNLK.L An */
uae_u32 REGPARAM2 CPUFUNC(op_4e58_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_areg (regs, srcreg);
	uae_s32 old = get_long_mmu060 (src);
	m68k_areg (regs, 7) = src + 4;
	m68k_areg (regs, srcreg) = old;
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* MVR2USP.L An */
uae_u32 REGPARAM2 CPUFUNC(op_4e60_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel15497; }
{{	uae_s32 src = m68k_areg (regs, srcreg);
	regs.usp = src;
}}}	m68k_incpci (2);
endlabel15497: ;
return 4 * CYCLE_UNIT / 2;
}

/* MVUSP2R.L An */
uae_u32 REGPARAM2 CPUFUNC(op_4e68_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel15498; }
{{	m68k_areg (regs, srcreg) = (regs.usp);
}}}	m68k_incpci (2);
endlabel15498: ;
return 4 * CYCLE_UNIT / 2;
}

/* RESET.L  */
uae_u32 REGPARAM2 CPUFUNC(op_4e70_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel15499; }
{	cpureset ();
	m68k_incpci (2);
}}endlabel15499: ;
return 4 * CYCLE_UNIT / 2;
}

/* NOP.L  */
uae_u32 REGPARAM2 CPUFUNC(op_4e71_33)(uae_u32 opcode)
{
{}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* STOP.L #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_4e72_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel15501; }
{{	uae_s16 src = get_iword_mmu060 (2);
	regs.sr = src;
	MakeFromSR ();
	m68k_setstopped ();
	m68k_incpci (4);
}}}endlabel15501: ;
return 8 * CYCLE_UNIT / 2;
}

/* RTE.L  */
uae_u32 REGPARAM2 CPUFUNC(op_4e73_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel15502; }
{	uae_u16 newsr; uae_u32 newpc;
	for (;;) {
		uaecptr a = m68k_areg (regs, 7);
		uae_u16 sr = get_word_mmu060 (a);
		uae_u32 pc = get_long_mmu060 (a + 2);
		uae_u16 format = get_word_mmu060 (a + 2 + 4);
		int frame = format >> 12;
		int offset = 8;
		newsr = sr; newpc = pc;
		if (frame == 0x0) { m68k_areg (regs, 7) += offset; break; }
		else if (frame == 0x1) { m68k_areg (regs, 7) += offset; }
		else if (frame == 0x2) { m68k_areg (regs, 7) += offset + 4; break; }
		else if (frame == 0x4) { m68k_do_rte_mmu060 (a); m68k_areg (regs, 7) += offset + 8; break; }
		else if (frame == 0x8) { m68k_areg (regs, 7) += offset + 50; break; }
		else if (frame == 0x7) { m68k_areg (regs, 7) += offset + 52; break; }
		else if (frame == 0x9) { m68k_areg (regs, 7) += offset + 12; break; }
		else if (frame == 0xa) { m68k_areg (regs, 7) += offset + 24; break; }
		else if (frame == 0xb) { m68k_areg (regs, 7) += offset + 84; break; }
		else { m68k_areg (regs, 7) += offset; Exception (14); goto endlabel15502; }
		regs.sr = newsr; MakeFromSR ();
}
	regs.sr = newsr; MakeFromSR ();
	if (newpc & 1) {
		exception3i (0x4E73, newpc);
		goto endlabel15502;
	}
	m68k_setpc_mmu (newpc);
	ipl_fetch ();
}}endlabel15502: ;
return 4 * CYCLE_UNIT / 2;
}

/* RTD.L #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_4e74_33)(uae_u32 opcode)
{
{{	uae_s16 offs = get_iword_mmu060 (2);
{	uaecptr pca;
	pca = m68k_areg (regs, 7);
{	uae_s32 pc = get_long_mmu060 (pca);
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) += 4;
	m68k_areg(regs, 7) += offs;
	if (pc & 1) {
		exception3i (0x4E74, pc);
		goto endlabel15503;
	}
	m68k_setpc_mmu (pc);
}}}}endlabel15503: ;
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* RTS.L  */
uae_u32 REGPARAM2 CPUFUNC(op_4e75_33)(uae_u32 opcode)
{
{	uaecptr pc = m68k_getpc ();
	m68k_do_rts_mmu060 ();
	if (m68k_getpc () & 1) {
		uaecptr faultpc = m68k_getpc ();
	m68k_setpc_mmu (pc);
		exception3i (0x4E75, faultpc);
	}
}return 4 * CYCLE_UNIT / 2;
}

/* TRAPV.L  */
uae_u32 REGPARAM2 CPUFUNC(op_4e76_33)(uae_u32 opcode)
{
{	m68k_incpci (2);
	if (GET_VFLG ()) {
		Exception (7);
		goto endlabel15505;
	}
}endlabel15505: ;
return 4 * CYCLE_UNIT / 2;
}

/* RTR.L  */
uae_u32 REGPARAM2 CPUFUNC(op_4e77_33)(uae_u32 opcode)
{
{	uaecptr oldpc = m68k_getpc ();
	MakeSR ();
{	uaecptr sra;
	sra = m68k_areg (regs, 7);
{	uae_s16 sr = get_word_mmu060 (sra);
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) += 2;
{	uaecptr pca;
	pca = m68k_areg (regs, 7);
{	uae_s32 pc = get_long_mmu060 (pca);
	mmufixup[1].reg = 7;
	mmufixup[1].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) += 4;
	regs.sr &= 0xFF00; sr &= 0xFF;
	regs.sr |= sr;
	m68k_setpc_mmu (pc);
	MakeFromSR ();
	if (m68k_getpc () & 1) {
		uaecptr faultpc = m68k_getpc ();
	m68k_setpc_mmu (oldpc);
		exception3i (0x4E77, faultpc);
	}
}}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVEC2.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4e7a_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel15507; }
{{	uae_s16 src = get_iword_mmu060 (2);
{	int regno = (src >> 12) & 15;
	uae_u32 *regp = regs.regs + regno;
	if (! m68k_movec2(src & 0xFFF, regp)) goto endlabel15507;
}}}}	m68k_incpci (4);
endlabel15507: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* MOVE2C.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4e7b_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel15508; }
{{	uae_s16 src = get_iword_mmu060 (2);
{	int regno = (src >> 12) & 15;
	uae_u32 *regp = regs.regs + regno;
	if (! m68k_move2c(src & 0xFFF, regp)) goto endlabel15508;
}}}}	m68k_incpci (4);
endlabel15508: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* JSR.L (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4e90_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uaecptr oldpc = m68k_getpc () + 2;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel15509;
	}
	put_long_mmu060 (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_setpc_mmu (srca);
}}}endlabel15509: ;
return 4 * CYCLE_UNIT / 2;
}

/* JSR.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ea8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uaecptr oldpc = m68k_getpc () + 4;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel15510;
	}
	put_long_mmu060 (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_setpc_mmu (srca);
}}}endlabel15510: ;
return 8 * CYCLE_UNIT / 2;
}

/* JSR.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4eb0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uaecptr oldpc = m68k_getpc () + 0;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel15511;
	}
	put_long_mmu060 (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_setpc_mmu (srca);
}}}}endlabel15511: ;
return 8 * CYCLE_UNIT / 2;
}

/* JSR.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4eb8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uaecptr oldpc = m68k_getpc () + 4;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel15512;
	}
	put_long_mmu060 (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_setpc_mmu (srca);
}}}endlabel15512: ;
return 8 * CYCLE_UNIT / 2;
}

/* JSR.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4eb9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uaecptr oldpc = m68k_getpc () + 6;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel15513;
	}
	put_long_mmu060 (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_setpc_mmu (srca);
}}}endlabel15513: ;
return 12 * CYCLE_UNIT / 2;
}

/* JSR.L (d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_4eba_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uaecptr oldpc = m68k_getpc () + 4;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel15514;
	}
	put_long_mmu060 (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_setpc_mmu (srca);
}}}endlabel15514: ;
return 8 * CYCLE_UNIT / 2;
}

/* JSR.L (d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4ebb_33)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uaecptr oldpc = m68k_getpc () + 0;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel15515;
	}
	put_long_mmu060 (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_setpc_mmu (srca);
}}}}endlabel15515: ;
return 8 * CYCLE_UNIT / 2;
}

/* JMP.L (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ed0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel15516;
	}
	m68k_setpc_mmu (srca);
}}endlabel15516: ;
return 4 * CYCLE_UNIT / 2;
}

/* JMP.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ee8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel15517;
	}
	m68k_setpc_mmu (srca);
}}endlabel15517: ;
return 8 * CYCLE_UNIT / 2;
}

/* JMP.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4ef0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel15518;
	}
	m68k_setpc_mmu (srca);
}}}endlabel15518: ;
return 8 * CYCLE_UNIT / 2;
}

/* JMP.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4ef8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel15519;
	}
	m68k_setpc_mmu (srca);
}}endlabel15519: ;
return 8 * CYCLE_UNIT / 2;
}

/* JMP.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4ef9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel15520;
	}
	m68k_setpc_mmu (srca);
}}endlabel15520: ;
return 12 * CYCLE_UNIT / 2;
}

/* JMP.L (d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_4efa_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel15521;
	}
	m68k_setpc_mmu (srca);
}}endlabel15521: ;
return 8 * CYCLE_UNIT / 2;
}

/* JMP.L (d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4efb_33)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel15522;
	}
	m68k_setpc_mmu (srca);
}}}endlabel15522: ;
return 8 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5000_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5010_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5018_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5020_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5028_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5030_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5038_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5039_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ADDQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5040_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDAQ.W #<data>,An */
uae_u32 REGPARAM2 CPUFUNC(op_5048_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDQ.W #<data>,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5050_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ADDQ.W #<data>,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5058_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ADDQ.W #<data>,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5060_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ADDQ.W #<data>,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5068_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADDQ.W #<data>,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5070_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

#endif

#ifdef PART_5
/* ADDQ.W #<data>,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5078_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADDQ.W #<data>,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5079_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5080_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDAQ.L #<data>,An */
uae_u32 REGPARAM2 CPUFUNC(op_5088_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5090_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
return 20 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5098_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_50a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_50a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_50b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_50b8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_50b9_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_50c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (0) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_50c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu060 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (0)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel15550;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel15550: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_50d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (0) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_50d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (0) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_50e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (0) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_50e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (0) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_50f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (0) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_50f8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (0) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_50f9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{{	int val = cctrue (0) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_50fa_33)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu060 (2);
	if (cctrue (0)) { Exception (7); goto endlabel15558; }
}}	m68k_incpci (4);
endlabel15558: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_50fb_33)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu060 (2);
	if (cctrue (0)) { Exception (7); goto endlabel15559; }
}}	m68k_incpci (6);
endlabel15559: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_50fc_33)(uae_u32 opcode)
{
{	if (cctrue (0)) { Exception (7); goto endlabel15560; }
}	m68k_incpci (2);
endlabel15560: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* SUBQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5100_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBQ.B #<data>,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5110_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* SUBQ.B #<data>,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5118_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* SUBQ.B #<data>,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5120_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* SUBQ.B #<data>,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5128_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUBQ.B #<data>,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5130_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUBQ.B #<data>,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5138_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUBQ.B #<data>,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5139_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5140_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBAQ.W #<data>,An */
uae_u32 REGPARAM2 CPUFUNC(op_5148_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5150_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5158_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5160_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5168_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5170_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5178_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5179_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5180_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBAQ.L #<data>,An */
uae_u32 REGPARAM2 CPUFUNC(op_5188_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5190_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
return 20 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5198_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_51a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_51a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_51b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_51b8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_51b9_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_51c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (1) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_51c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu060 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (1)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel15588;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel15588: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_51d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (1) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_51d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (1) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_51e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (1) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_51e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (1) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_51f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (1) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_51f8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (1) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_51f9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{{	int val = cctrue (1) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_51fa_33)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu060 (2);
	if (cctrue (1)) { Exception (7); goto endlabel15596; }
}}	m68k_incpci (4);
endlabel15596: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_51fb_33)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu060 (2);
	if (cctrue (1)) { Exception (7); goto endlabel15597; }
}}	m68k_incpci (6);
endlabel15597: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_51fc_33)(uae_u32 opcode)
{
{	if (cctrue (1)) { Exception (7); goto endlabel15598; }
}	m68k_incpci (2);
endlabel15598: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_52c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (2) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_52c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu060 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (2)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel15600;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel15600: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_52d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (2) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_52d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (2) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_52e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (2) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_52e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (2) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_52f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (2) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_52f8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (2) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_52f9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{{	int val = cctrue (2) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_52fa_33)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu060 (2);
	if (cctrue (2)) { Exception (7); goto endlabel15608; }
}}	m68k_incpci (4);
endlabel15608: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_52fb_33)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu060 (2);
	if (cctrue (2)) { Exception (7); goto endlabel15609; }
}}	m68k_incpci (6);
endlabel15609: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_52fc_33)(uae_u32 opcode)
{
{	if (cctrue (2)) { Exception (7); goto endlabel15610; }
}	m68k_incpci (2);
endlabel15610: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_53c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (3) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_53c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu060 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (3)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel15612;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel15612: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_53d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (3) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_53d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (3) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_53e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (3) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_53e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (3) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_53f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (3) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_53f8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (3) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_53f9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{{	int val = cctrue (3) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_53fa_33)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu060 (2);
	if (cctrue (3)) { Exception (7); goto endlabel15620; }
}}	m68k_incpci (4);
endlabel15620: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_53fb_33)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu060 (2);
	if (cctrue (3)) { Exception (7); goto endlabel15621; }
}}	m68k_incpci (6);
endlabel15621: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_53fc_33)(uae_u32 opcode)
{
{	if (cctrue (3)) { Exception (7); goto endlabel15622; }
}	m68k_incpci (2);
endlabel15622: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_54c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (4) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_54c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu060 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (4)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel15624;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel15624: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_54d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (4) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_54d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (4) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_54e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (4) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_54e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (4) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_54f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (4) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_54f8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (4) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_54f9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{{	int val = cctrue (4) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_54fa_33)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu060 (2);
	if (cctrue (4)) { Exception (7); goto endlabel15632; }
}}	m68k_incpci (4);
endlabel15632: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_54fb_33)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu060 (2);
	if (cctrue (4)) { Exception (7); goto endlabel15633; }
}}	m68k_incpci (6);
endlabel15633: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_54fc_33)(uae_u32 opcode)
{
{	if (cctrue (4)) { Exception (7); goto endlabel15634; }
}	m68k_incpci (2);
endlabel15634: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_55c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (5) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_55c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu060 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (5)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel15636;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel15636: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_55d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (5) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_55d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (5) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_55e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (5) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_55e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (5) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_55f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (5) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_55f8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (5) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_55f9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{{	int val = cctrue (5) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_55fa_33)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu060 (2);
	if (cctrue (5)) { Exception (7); goto endlabel15644; }
}}	m68k_incpci (4);
endlabel15644: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_55fb_33)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu060 (2);
	if (cctrue (5)) { Exception (7); goto endlabel15645; }
}}	m68k_incpci (6);
endlabel15645: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_55fc_33)(uae_u32 opcode)
{
{	if (cctrue (5)) { Exception (7); goto endlabel15646; }
}	m68k_incpci (2);
endlabel15646: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_56c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (6) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_56c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu060 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (6)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel15648;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel15648: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_56d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (6) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_56d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (6) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_56e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (6) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_56e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (6) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_56f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (6) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_56f8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (6) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_56f9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{{	int val = cctrue (6) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_56fa_33)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu060 (2);
	if (cctrue (6)) { Exception (7); goto endlabel15656; }
}}	m68k_incpci (4);
endlabel15656: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_56fb_33)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu060 (2);
	if (cctrue (6)) { Exception (7); goto endlabel15657; }
}}	m68k_incpci (6);
endlabel15657: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_56fc_33)(uae_u32 opcode)
{
{	if (cctrue (6)) { Exception (7); goto endlabel15658; }
}	m68k_incpci (2);
endlabel15658: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_57c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (7) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_57c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu060 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (7)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel15660;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel15660: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_57d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (7) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_57d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (7) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_57e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (7) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_57e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (7) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_57f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (7) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_57f8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (7) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_57f9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{{	int val = cctrue (7) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_57fa_33)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu060 (2);
	if (cctrue (7)) { Exception (7); goto endlabel15668; }
}}	m68k_incpci (4);
endlabel15668: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_57fb_33)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu060 (2);
	if (cctrue (7)) { Exception (7); goto endlabel15669; }
}}	m68k_incpci (6);
endlabel15669: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_57fc_33)(uae_u32 opcode)
{
{	if (cctrue (7)) { Exception (7); goto endlabel15670; }
}	m68k_incpci (2);
endlabel15670: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_58c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (8) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_58c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu060 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (8)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel15672;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel15672: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_58d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (8) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_58d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (8) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_58e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (8) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_58e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (8) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_58f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (8) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_58f8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (8) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_58f9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{{	int val = cctrue (8) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_58fa_33)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu060 (2);
	if (cctrue (8)) { Exception (7); goto endlabel15680; }
}}	m68k_incpci (4);
endlabel15680: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_58fb_33)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu060 (2);
	if (cctrue (8)) { Exception (7); goto endlabel15681; }
}}	m68k_incpci (6);
endlabel15681: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_58fc_33)(uae_u32 opcode)
{
{	if (cctrue (8)) { Exception (7); goto endlabel15682; }
}	m68k_incpci (2);
endlabel15682: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_59c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (9) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_59c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu060 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (9)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel15684;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel15684: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_59d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (9) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_59d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (9) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_59e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (9) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_59e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (9) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_59f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (9) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_59f8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (9) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_59f9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{{	int val = cctrue (9) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_59fa_33)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu060 (2);
	if (cctrue (9)) { Exception (7); goto endlabel15692; }
}}	m68k_incpci (4);
endlabel15692: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_59fb_33)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu060 (2);
	if (cctrue (9)) { Exception (7); goto endlabel15693; }
}}	m68k_incpci (6);
endlabel15693: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_59fc_33)(uae_u32 opcode)
{
{	if (cctrue (9)) { Exception (7); goto endlabel15694; }
}	m68k_incpci (2);
endlabel15694: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5ac0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (10) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_5ac8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu060 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (10)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel15696;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel15696: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ad0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (10) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5ad8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (10) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ae0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (10) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ae8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (10) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5af0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (10) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5af8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (10) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5af9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{{	int val = cctrue (10) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5afa_33)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu060 (2);
	if (cctrue (10)) { Exception (7); goto endlabel15704; }
}}	m68k_incpci (4);
endlabel15704: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5afb_33)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu060 (2);
	if (cctrue (10)) { Exception (7); goto endlabel15705; }
}}	m68k_incpci (6);
endlabel15705: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5afc_33)(uae_u32 opcode)
{
{	if (cctrue (10)) { Exception (7); goto endlabel15706; }
}	m68k_incpci (2);
endlabel15706: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5bc0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (11) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_5bc8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu060 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (11)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel15708;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel15708: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_5bd0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (11) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5bd8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (11) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5be0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (11) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5be8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (11) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5bf0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (11) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5bf8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (11) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5bf9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{{	int val = cctrue (11) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5bfa_33)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu060 (2);
	if (cctrue (11)) { Exception (7); goto endlabel15716; }
}}	m68k_incpci (4);
endlabel15716: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5bfb_33)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu060 (2);
	if (cctrue (11)) { Exception (7); goto endlabel15717; }
}}	m68k_incpci (6);
endlabel15717: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5bfc_33)(uae_u32 opcode)
{
{	if (cctrue (11)) { Exception (7); goto endlabel15718; }
}	m68k_incpci (2);
endlabel15718: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5cc0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (12) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_5cc8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu060 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (12)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel15720;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel15720: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_5cd0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (12) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5cd8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (12) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ce0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (12) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ce8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (12) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5cf0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (12) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5cf8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (12) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5cf9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{{	int val = cctrue (12) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5cfa_33)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu060 (2);
	if (cctrue (12)) { Exception (7); goto endlabel15728; }
}}	m68k_incpci (4);
endlabel15728: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5cfb_33)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu060 (2);
	if (cctrue (12)) { Exception (7); goto endlabel15729; }
}}	m68k_incpci (6);
endlabel15729: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5cfc_33)(uae_u32 opcode)
{
{	if (cctrue (12)) { Exception (7); goto endlabel15730; }
}	m68k_incpci (2);
endlabel15730: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5dc0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (13) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_5dc8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu060 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (13)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel15732;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel15732: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_5dd0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (13) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5dd8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (13) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5de0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (13) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5de8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (13) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5df0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (13) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5df8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (13) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5df9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{{	int val = cctrue (13) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5dfa_33)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu060 (2);
	if (cctrue (13)) { Exception (7); goto endlabel15740; }
}}	m68k_incpci (4);
endlabel15740: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5dfb_33)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu060 (2);
	if (cctrue (13)) { Exception (7); goto endlabel15741; }
}}	m68k_incpci (6);
endlabel15741: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5dfc_33)(uae_u32 opcode)
{
{	if (cctrue (13)) { Exception (7); goto endlabel15742; }
}	m68k_incpci (2);
endlabel15742: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5ec0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (14) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_5ec8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu060 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (14)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel15744;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel15744: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ed0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (14) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5ed8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (14) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ee0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (14) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ee8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (14) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5ef0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (14) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5ef8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (14) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5ef9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{{	int val = cctrue (14) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5efa_33)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu060 (2);
	if (cctrue (14)) { Exception (7); goto endlabel15752; }
}}	m68k_incpci (4);
endlabel15752: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5efb_33)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu060 (2);
	if (cctrue (14)) { Exception (7); goto endlabel15753; }
}}	m68k_incpci (6);
endlabel15753: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5efc_33)(uae_u32 opcode)
{
{	if (cctrue (14)) { Exception (7); goto endlabel15754; }
}	m68k_incpci (2);
endlabel15754: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5fc0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (15) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_5fc8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu060 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (15)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel15756;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel15756: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_5fd0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (15) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5fd8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (15) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5fe0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (15) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5fe8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (15) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5ff0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (15) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5ff8_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{{	int val = cctrue (15) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5ff9_33)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{{	int val = cctrue (15) ? 0xff : 0;
	put_byte_mmu060 (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5ffa_33)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu060 (2);
	if (cctrue (15)) { Exception (7); goto endlabel15764; }
}}	m68k_incpci (4);
endlabel15764: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5ffb_33)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu060 (2);
	if (cctrue (15)) { Exception (7); goto endlabel15765; }
}}	m68k_incpci (6);
endlabel15765: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5ffc_33)(uae_u32 opcode)
{
{	if (cctrue (15)) { Exception (7); goto endlabel15766; }
}	m68k_incpci (2);
endlabel15766: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6000_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
	if (!cctrue (0)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15767;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel15767: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6001_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (0)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15768;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel15768: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_60ff_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
	if (!cctrue (0)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15769;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel15769: ;
return 12 * CYCLE_UNIT / 2;
}

/* BSR.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6100_33)(uae_u32 opcode)
{
{	uae_s32 s;
{	uae_s16 src = get_iword_mmu060 (2);
	s = (uae_s32)src + 2;
	if (src & 1) {
		exception3pc (opcode, m68k_getpc () + s, 0, 1, m68k_getpc () + s);
		goto endlabel15770;
	}
	m68k_do_bsr_mmu060 (m68k_getpc () + 4, s);
}}endlabel15770: ;
return 8 * CYCLE_UNIT / 2;
}

/* BSRQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6101_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{	uae_s32 s;
{	uae_u32 src = srcreg;
	s = (uae_s32)src + 2;
	if (src & 1) {
		exception3pc (opcode, m68k_getpc () + s, 0, 1, m68k_getpc () + s);
		goto endlabel15771;
	}
	m68k_do_bsr_mmu060 (m68k_getpc () + 2, s);
}}endlabel15771: ;
return 4 * CYCLE_UNIT / 2;
}

/* BSR.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_61ff_33)(uae_u32 opcode)
{
{	uae_s32 s;
{	uae_s32 src;
	src = get_ilong_mmu060 (2);
	s = (uae_s32)src + 2;
	if (src & 1) {
		exception3pc (opcode, m68k_getpc () + s, 0, 1, m68k_getpc () + s);
		goto endlabel15772;
	}
	m68k_do_bsr_mmu060 (m68k_getpc () + 6, s);
}}endlabel15772: ;
return 12 * CYCLE_UNIT / 2;
}

#endif

#ifdef PART_6
/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6200_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
	if (!cctrue (2)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15773;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel15773: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6201_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (2)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15774;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel15774: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_62ff_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
	if (!cctrue (2)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15775;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel15775: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6300_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
	if (!cctrue (3)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15776;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel15776: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6301_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (3)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15777;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel15777: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_63ff_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
	if (!cctrue (3)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15778;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel15778: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6400_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
	if (!cctrue (4)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15779;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel15779: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6401_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (4)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15780;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel15780: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_64ff_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
	if (!cctrue (4)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15781;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel15781: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6500_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
	if (!cctrue (5)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15782;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel15782: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6501_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (5)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15783;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel15783: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_65ff_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
	if (!cctrue (5)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15784;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel15784: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6600_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
	if (!cctrue (6)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15785;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel15785: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6601_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (6)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15786;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel15786: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_66ff_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
	if (!cctrue (6)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15787;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel15787: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6700_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
	if (!cctrue (7)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15788;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel15788: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6701_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (7)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15789;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel15789: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_67ff_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
	if (!cctrue (7)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15790;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel15790: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6800_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
	if (!cctrue (8)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15791;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel15791: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6801_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (8)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15792;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel15792: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_68ff_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
	if (!cctrue (8)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15793;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel15793: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6900_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
	if (!cctrue (9)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15794;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel15794: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6901_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (9)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15795;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel15795: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_69ff_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
	if (!cctrue (9)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15796;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel15796: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6a00_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
	if (!cctrue (10)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15797;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel15797: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6a01_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (10)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15798;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel15798: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_6aff_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
	if (!cctrue (10)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15799;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel15799: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6b00_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
	if (!cctrue (11)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15800;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel15800: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6b01_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (11)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15801;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel15801: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_6bff_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
	if (!cctrue (11)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15802;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel15802: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6c00_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
	if (!cctrue (12)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15803;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel15803: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6c01_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (12)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15804;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel15804: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_6cff_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
	if (!cctrue (12)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15805;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel15805: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6d00_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
	if (!cctrue (13)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15806;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel15806: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6d01_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (13)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15807;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel15807: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_6dff_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
	if (!cctrue (13)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15808;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel15808: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6e00_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
	if (!cctrue (14)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15809;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel15809: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6e01_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (14)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15810;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel15810: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_6eff_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
	if (!cctrue (14)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15811;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel15811: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6f00_33)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu060 (2);
	if (!cctrue (15)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15812;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel15812: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6f01_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (15)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15813;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel15813: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_6fff_33)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
	if (!cctrue (15)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel15814;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel15814: ;
return 12 * CYCLE_UNIT / 2;
}

/* MOVEQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_7000_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_u32 src = srcreg;
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* OR.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8000_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* OR.B (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8010_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* OR.B (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8018_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* OR.B -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8020_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* OR.B (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8028_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.B (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8030_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* OR.B (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8038_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.B (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8039_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* OR.B (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_803a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.B (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_803b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_803c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8040_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* OR.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8050_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* OR.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8058_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* OR.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8060_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* OR.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8068_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8070_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* OR.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8078_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8079_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* OR.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_807a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_807b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_807c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* OR.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8080_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* OR.L (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8090_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* OR.L (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8098_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* OR.L -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* OR.L (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* OR.L (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* OR.L (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80b8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* OR.L (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80b9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* OR.L (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80ba_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* OR.L (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80bb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80bc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* DIVU.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel15849;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpci (2);
	}
}}}endlabel15849: ;
return 110 * CYCLE_UNIT / 2;
}

/* DIVU.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel15850;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpci (2);
	}
}}}}endlabel15850: ;
return 114 * CYCLE_UNIT / 2;
}

/* DIVU.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel15851;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpci (2);
	}
}}}}endlabel15851: ;
	mmufixup[0].reg = -1;
return 114 * CYCLE_UNIT / 2;
}

/* DIVU.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel15852;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpci (2);
	}
}}}}endlabel15852: ;
	mmufixup[0].reg = -1;
return 116 * CYCLE_UNIT / 2;
}

/* DIVU.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel15853;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpci (4);
	}
}}}}endlabel15853: ;
return 118 * CYCLE_UNIT / 2;
}

/* DIVU.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (0);
		Exception (5);
		goto endlabel15854;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
}}}}}endlabel15854: ;
return 118 * CYCLE_UNIT / 2;
}

/* DIVU.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80f8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel15855;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpci (4);
	}
}}}}endlabel15855: ;
return 118 * CYCLE_UNIT / 2;
}

/* DIVU.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80f9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (6);
		Exception (5);
		goto endlabel15856;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpci (6);
	}
}}}}endlabel15856: ;
return 122 * CYCLE_UNIT / 2;
}

/* DIVU.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80fa_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel15857;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpci (4);
	}
}}}}endlabel15857: ;
return 118 * CYCLE_UNIT / 2;
}

/* DIVU.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80fb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (0);
		Exception (5);
		goto endlabel15858;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
}}}}}endlabel15858: ;
return 118 * CYCLE_UNIT / 2;
}

/* DIVU.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80fc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel15859;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpci (4);
	}
}}}endlabel15859: ;
return 114 * CYCLE_UNIT / 2;
}

/* SBCD.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8100_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	uae_u16 newv_lo = (dst & 0xF) - (src & 0xF) - (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = (dst & 0xF0) - (src & 0xF0);
	uae_u16 newv, tmp_newv;
	int bcd = 0;
	newv = tmp_newv = newv_hi + newv_lo;
	if (newv_lo & 0xF0) { newv -= 6; bcd = 6; };
	if ((((dst & 0xFF) - (src & 0xFF) - (GET_XFLG () ? 1 : 0)) & 0x100) > 0xFF) { newv -= 0x60; }
	SET_CFLG ((((dst & 0xFF) - (src & 0xFF) - bcd - (GET_XFLG () ? 1 : 0)) & 0x300) > 0xFF);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SBCD.B -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_8108_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	uae_u16 newv_lo = (dst & 0xF) - (src & 0xF) - (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = (dst & 0xF0) - (src & 0xF0);
	uae_u16 newv, tmp_newv;
	int bcd = 0;
	newv = tmp_newv = newv_hi + newv_lo;
	if (newv_lo & 0xF0) { newv -= 6; bcd = 6; };
	if ((((dst & 0xFF) - (src & 0xFF) - (GET_XFLG () ? 1 : 0)) & 0x100) > 0xFF) { newv -= 0x60; }
	SET_CFLG ((((dst & 0xFF) - (src & 0xFF) - bcd - (GET_XFLG () ? 1 : 0)) & 0x300) > 0xFF);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* OR.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_8110_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* OR.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_8118_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* OR.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_8120_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* OR.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_8128_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* OR.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_8130_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* OR.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_8138_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* OR.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_8139_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* PACK.L Dn,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_8140_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uae_u16 val = m68k_dreg (regs, srcreg) + get_iword_mmu060 (2);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & 0xffffff00) | ((val >> 4) & 0xf0) | (val & 0xf);
}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* PACK.L -(An),-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_8148_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uae_u16 val;
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) -= areg_byteinc[srcreg];
	val = (uae_u16)get_byte_mmu060 (m68k_areg (regs, srcreg));
	m68k_areg (regs, srcreg) -= areg_byteinc[srcreg];
	val = (val | ((uae_u16)get_byte_mmu060 (m68k_areg (regs, srcreg)) << 8)) + get_iword_mmu060 (2);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) -= areg_byteinc[dstreg];
	put_byte_mmu060 (m68k_areg (regs, dstreg),((val >> 4) & 0xf0) | (val & 0xf));
}	m68k_incpci (4);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* OR.W Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_8150_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* OR.W Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_8158_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* OR.W Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_8160_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* OR.W Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_8168_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* OR.W Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_8170_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* OR.W Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_8178_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* OR.W Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_8179_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* UNPK.L Dn,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_8180_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uae_u16 val = m68k_dreg (regs, srcreg);
	val = (((val << 4) & 0xf00) | (val & 0xf)) + get_iword_mmu060 (2);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & 0xffff0000) | (val & 0xffff);
}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* UNPK.L -(An),-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_8188_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uae_u16 val;
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) -= areg_byteinc[srcreg];
	val = (uae_u16)get_byte_mmu060 (m68k_areg (regs, srcreg));
	val = (((val << 4) & 0xf00) | (val & 0xf)) + get_iword_mmu060 (2);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) -= 2 * areg_byteinc[dstreg];
	put_byte_mmu060 (m68k_areg (regs, dstreg) + areg_byteinc[dstreg], val);
	put_byte_mmu060 (m68k_areg (regs, dstreg), val >> 8);
}	m68k_incpci (4);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* OR.L Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_8190_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
return 20 * CYCLE_UNIT / 2;
}

/* OR.L Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_8198_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* OR.L Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_81a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* OR.L Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_81a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* OR.L Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_81b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* OR.L Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_81b8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* OR.L Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_81b9_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* DIVS.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel15887;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
	m68k_incpci (2);
}}}endlabel15887: ;
return 142 * CYCLE_UNIT / 2;
}

/* DIVS.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel15888;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
	m68k_incpci (2);
}}}}endlabel15888: ;
return 146 * CYCLE_UNIT / 2;
}

/* DIVS.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel15889;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
	m68k_incpci (2);
}}}}endlabel15889: ;
	mmufixup[0].reg = -1;
return 146 * CYCLE_UNIT / 2;
}

/* DIVS.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel15890;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
	m68k_incpci (2);
}}}}endlabel15890: ;
	mmufixup[0].reg = -1;
return 148 * CYCLE_UNIT / 2;
}

/* DIVS.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel15891;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
	m68k_incpci (4);
}}}}endlabel15891: ;
return 150 * CYCLE_UNIT / 2;
}

/* DIVS.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (0);
		Exception (5);
		goto endlabel15892;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
}}}}}endlabel15892: ;
return 150 * CYCLE_UNIT / 2;
}

/* DIVS.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81f8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel15893;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
	m68k_incpci (4);
}}}}endlabel15893: ;
return 150 * CYCLE_UNIT / 2;
}

/* DIVS.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81f9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (6);
		Exception (5);
		goto endlabel15894;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
	m68k_incpci (6);
}}}}endlabel15894: ;
return 154 * CYCLE_UNIT / 2;
}

/* DIVS.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81fa_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel15895;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
	m68k_incpci (4);
}}}}endlabel15895: ;
return 150 * CYCLE_UNIT / 2;
}

/* DIVS.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81fb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (0);
		Exception (5);
		goto endlabel15896;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
}}}}}endlabel15896: ;
return 150 * CYCLE_UNIT / 2;
}

/* DIVS.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81fc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel15897;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
	m68k_incpci (4);
}}}endlabel15897: ;
return 146 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9000_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUB.B (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9010_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* SUB.B (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9018_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* SUB.B -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9020_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* SUB.B (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9028_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.B (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9030_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUB.B (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9038_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.B (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9039_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.B (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_903a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.B (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_903b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_903c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9040_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUB.W An,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9048_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUB.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9050_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* SUB.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9058_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* SUB.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9060_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* SUB.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9068_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9070_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUB.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9078_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9079_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_907a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_907b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_907c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9080_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUB.L An,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9088_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUB.L (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9090_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.L (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9098_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* SUB.L -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* SUB.L (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.L (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUB.L (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90b8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.L (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90b9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* SUB.L (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90ba_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.L (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90bb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90bc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* SUBA.W Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_90c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBA.W An,An */
uae_u32 REGPARAM2 CPUFUNC(op_90c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBA.W (An),An */
uae_u32 REGPARAM2 CPUFUNC(op_90d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* SUBA.W (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_90d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* SUBA.W -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_90e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* SUBA.W (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_90e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUBA.W (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_90f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUBA.W (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_90f8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUBA.W (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_90f9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* SUBA.W (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_90fa_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUBA.W (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_90fb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUBA.W #<data>.W,An */
uae_u32 REGPARAM2 CPUFUNC(op_90fc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* SUBX.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9100_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = dst - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBX.B -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_9108_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	uae_u32 newv = dst - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_9110_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_9118_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_9120_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_9128_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_9130_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_9138_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_9139_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* SUBX.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9140_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = dst - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBX.W -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_9148_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	uae_u32 newv = dst - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_9150_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_9158_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_9160_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_9168_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_9170_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_9178_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_9179_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* SUBX.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9180_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = dst - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBX.L -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_9188_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	uae_u32 newv = dst - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_9190_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
return 20 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_9198_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_91a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_91a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_91b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_91b8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_91b9_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* SUBA.L Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_91c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBA.L An,An */
uae_u32 REGPARAM2 CPUFUNC(op_91c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBA.L (An),An */
uae_u32 REGPARAM2 CPUFUNC(op_91d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* SUBA.L (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_91d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* SUBA.L -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_91e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* SUBA.L (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_91e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUBA.L (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_91f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUBA.L (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_91f8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUBA.L (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_91f9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* SUBA.L (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_91fa_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUBA.L (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_91fb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUBA.L #<data>.L,An */
uae_u32 REGPARAM2 CPUFUNC(op_91fc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b000_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMP.B (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b010_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* CMP.B (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b018_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* CMP.B -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b020_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* CMP.B (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b028_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.B (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b030_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* CMP.B (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b038_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.B (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b039_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.B (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b03a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.B (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b03b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b03c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b040_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMP.W An,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b048_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMP.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b050_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* CMP.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b058_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* CMP.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b060_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* CMP.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b068_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b070_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* CMP.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b078_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b079_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b07a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b07b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b07c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* CMP.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b080_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMP.L An,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b088_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

#endif

#ifdef PART_7
/* CMP.L (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b090_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.L (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b098_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* CMP.L -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* CMP.L (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.L (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* CMP.L (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0b8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.L (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0b9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* CMP.L (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0ba_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.L (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0bb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0bc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* CMPA.W Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_b0c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMPA.W An,An */
uae_u32 REGPARAM2 CPUFUNC(op_b0c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMPA.W (An),An */
uae_u32 REGPARAM2 CPUFUNC(op_b0d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* CMPA.W (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_b0d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* CMPA.W -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_b0e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* CMPA.W (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_b0e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMPA.W (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_b0f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* CMPA.W (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_b0f8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMPA.W (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_b0f9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* CMPA.W (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_b0fa_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMPA.W (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_b0fb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* CMPA.W #<data>.W,An */
uae_u32 REGPARAM2 CPUFUNC(op_b0fc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b100_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMPM.B (An)+,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_b108_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu060 (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_b110_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_b118_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_b120_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_b128_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_b130_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_b138_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_b139_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b140_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMPM.W (An)+,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_b148_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu060 (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_b150_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_b158_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_b160_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_b168_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_b170_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_b178_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_b179_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b180_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMPM.L (An)+,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_b188_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu060 (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_b190_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
return 20 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_b198_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_b1a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_b1a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_b1b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_b1b8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_b1b9_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* CMPA.L Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_b1c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMPA.L An,An */
uae_u32 REGPARAM2 CPUFUNC(op_b1c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMPA.L (An),An */
uae_u32 REGPARAM2 CPUFUNC(op_b1d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* CMPA.L (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_b1d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* CMPA.L -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_b1e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* CMPA.L (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_b1e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CMPA.L (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_b1f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* CMPA.L (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_b1f8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CMPA.L (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_b1f9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* CMPA.L (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_b1fa_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CMPA.L (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_b1fb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* CMPA.L #<data>.L,An */
uae_u32 REGPARAM2 CPUFUNC(op_b1fc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* AND.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c000_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* AND.B (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c010_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* AND.B (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c018_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* AND.B -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c020_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* AND.B (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c028_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.B (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c030_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* AND.B (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c038_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.B (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c039_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* AND.B (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c03a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.B (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c03b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c03c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c040_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* AND.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c050_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* AND.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c058_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* AND.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c060_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* AND.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c068_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c070_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* AND.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c078_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c079_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* AND.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c07a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c07b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c07c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* AND.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c080_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* AND.L (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c090_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* AND.L (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c098_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* AND.L -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* AND.L (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* AND.L (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.L (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0b8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* AND.L (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0b9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* AND.L (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0ba_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* AND.L (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0bb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0bc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MULU.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (2);
}}}}return 58 * CYCLE_UNIT / 2;
}

/* MULU.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (2);
}}}}}return 62 * CYCLE_UNIT / 2;
}

/* MULU.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (2);
}}}}}	mmufixup[0].reg = -1;
return 62 * CYCLE_UNIT / 2;
}

/* MULU.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (2);
}}}}}	mmufixup[0].reg = -1;
return 64 * CYCLE_UNIT / 2;
}

/* MULU.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (4);
}}}}}return 66 * CYCLE_UNIT / 2;
}

/* MULU.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}return 66 * CYCLE_UNIT / 2;
}

/* MULU.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0f8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (4);
}}}}}return 66 * CYCLE_UNIT / 2;
}

/* MULU.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0f9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (6);
}}}}}return 70 * CYCLE_UNIT / 2;
}

/* MULU.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0fa_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (4);
}}}}}return 66 * CYCLE_UNIT / 2;
}

/* MULU.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0fb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}return 66 * CYCLE_UNIT / 2;
}

/* MULU.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0fc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (4);
}}}}return 62 * CYCLE_UNIT / 2;
}

/* ABCD.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c100_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	uae_u16 newv_lo = (src & 0xF) + (dst & 0xF) + (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = (src & 0xF0) + (dst & 0xF0);
	uae_u16 newv, tmp_newv;
	int cflg;
	newv = tmp_newv = newv_hi + newv_lo;	if (newv_lo > 9) { newv += 6; }
	cflg = (newv & 0x3F0) > 0x90;
	if (cflg) newv += 0x60;
	SET_CFLG (cflg);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ABCD.B -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_c108_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	uae_u16 newv_lo = (src & 0xF) + (dst & 0xF) + (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = (src & 0xF0) + (dst & 0xF0);
	uae_u16 newv, tmp_newv;
	int cflg;
	newv = tmp_newv = newv_hi + newv_lo;	if (newv_lo > 9) { newv += 6; }
	cflg = (newv & 0x3F0) > 0x90;
	if (cflg) newv += 0x60;
	SET_CFLG (cflg);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* AND.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_c110_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* AND.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_c118_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* AND.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_c120_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* AND.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_c128_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* AND.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_c130_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_c138_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* AND.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_c139_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_rmw_byte_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* EXG.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c140_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_dreg (regs, srcreg) = (dst);
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* EXG.L An,An */
uae_u32 REGPARAM2 CPUFUNC(op_c148_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	m68k_areg (regs, srcreg) = (dst);
	m68k_areg (regs, dstreg) = (src);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* AND.W Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_c150_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* AND.W Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_c158_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* AND.W Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_c160_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* AND.W Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_c168_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* AND.W Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_c170_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.W Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_c178_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* AND.W Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_c179_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_rmw_word_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* EXG.L Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_c188_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	m68k_dreg (regs, srcreg) = (dst);
	m68k_areg (regs, dstreg) = (src);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* AND.L Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_c190_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
return 20 * CYCLE_UNIT / 2;
}

/* AND.L Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_c198_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* AND.L Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_c1a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* AND.L Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_c1a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* AND.L Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_c1b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* AND.L Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_c1b8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* AND.L Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_c1b9_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_rmw_long_mmu060 (dsta, src);
}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* MULS.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 58 * CYCLE_UNIT / 2;
}

/* MULS.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 62 * CYCLE_UNIT / 2;
}

/* MULS.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 62 * CYCLE_UNIT / 2;
}

/* MULS.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 64 * CYCLE_UNIT / 2;
}

/* MULS.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 66 * CYCLE_UNIT / 2;
}

/* MULS.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}return 66 * CYCLE_UNIT / 2;
}

/* MULS.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1f8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 66 * CYCLE_UNIT / 2;
}

/* MULS.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1f9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 70 * CYCLE_UNIT / 2;
}

/* MULS.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1fa_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 66 * CYCLE_UNIT / 2;
}

/* MULS.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1fb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}return 66 * CYCLE_UNIT / 2;
}

/* MULS.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1fc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}	m68k_incpci (4);
return 62 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d000_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADD.B (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d010_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* ADD.B (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d018_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* ADD.B -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d020_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* ADD.B (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d028_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.B (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d030_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADD.B (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d038_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.B (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d039_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.B (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d03a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.B (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d03b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu060 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d03c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu060 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d040_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADD.W An,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d048_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADD.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d050_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* ADD.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d058_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* ADD.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d060_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* ADD.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d068_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d070_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADD.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d078_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d079_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d07a_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d07b_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d07c_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d080_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADD.L An,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d088_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADD.L (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d090_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.L (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d098_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ADD.L -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ADD.L (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.L (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADD.L (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0b8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.L (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0b9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ADD.L (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0ba_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.L (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0bb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0bc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* ADDA.W Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_d0c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDA.W An,An */
uae_u32 REGPARAM2 CPUFUNC(op_d0c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDA.W (An),An */
uae_u32 REGPARAM2 CPUFUNC(op_d0d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* ADDA.W (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_d0d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* ADDA.W -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_d0e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* ADDA.W (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_d0e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADDA.W (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_d0f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADDA.W (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_d0f8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADDA.W (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_d0f9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* ADDA.W (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_d0fa_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADDA.W (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_d0fb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADDA.W #<data>.W,An */
uae_u32 REGPARAM2 CPUFUNC(op_d0fc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu060 (2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* ADDX.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d100_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = dst + src + (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgn) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgo) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDX.B -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d108_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	uae_u32 newv = dst + src + (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgn) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgo) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d110_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_d118_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d120_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_d128_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_d130_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_d138_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_d139_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s8 dst = get_rmw_byte_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_rmw_byte_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ADDX.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d140_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = dst + src + (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgn) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgo) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDX.W -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d148_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	uae_u32 newv = dst + src + (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgn) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgo) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d150_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_d158_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d160_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_d168_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_d170_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_d178_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_d179_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s16 dst = get_rmw_word_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_rmw_word_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ADDX.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d180_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = dst + src + (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgn) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgo) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDX.L -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d188_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	uae_u32 newv = dst + src + (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgn) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgo) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d190_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
return 20 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_d198_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d1a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_d1a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_d1b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_d1b8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_d1b9_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (2);
{	uae_s32 dst = get_rmw_long_mmu060 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_rmw_long_mmu060 (dsta, newv);
}}}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* ADDA.L Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_d1c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDA.L An,An */
uae_u32 REGPARAM2 CPUFUNC(op_d1c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDA.L (An),An */
uae_u32 REGPARAM2 CPUFUNC(op_d1d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ADDA.L (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_d1d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ADDA.L -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_d1e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu060 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ADDA.L (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_d1e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADDA.L (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_d1f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADDA.L (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_d1f8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADDA.L (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_d1f9_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ADDA.L (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_d1fa_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADDA.L (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_d1fb_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu060 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADDA.L #<data>.L,An */
uae_u32 REGPARAM2 CPUFUNC(op_d1fc_33)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu060 (2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* ASRQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e000_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	uae_u32 sign = (0x80 & val) >> 7;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 8) {
		val = 0xff & (uae_u32)-sign;
		SET_CFLG (sign);
		COPY_CARRY ();
	} else {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
		val |= (0xff << (8 - cnt)) & (uae_u32)-sign;
		val &= 0xff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSRQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e008_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 8) {
		SET_CFLG ((cnt == 8) & (val >> 7));
		COPY_CARRY ();
		val = 0;
	} else {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXRQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e010_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
{	cnt--;
	{
	uae_u32 carry;
	uae_u32 hival = (val << 1) | GET_XFLG ();
	hival <<= (7 - cnt);
	val >>= cnt;
	carry = val & 1;
	val >>= 1;
	val |= hival;
	SET_XFLG (carry);
	val &= 0xff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* RORQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e018_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
{	uae_u32 hival;
	cnt &= 7;
	hival = val << (8 - cnt);
	val >>= cnt;
	val |= hival;
	val &= 0xff;
	SET_CFLG ((val & 0x80) >> 7);
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASR.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e020_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	uae_u32 sign = (0x80 & val) >> 7;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 8) {
		val = 0xff & (uae_u32)-sign;
		SET_CFLG (sign);
		COPY_CARRY ();
	} else if (cnt > 0) {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
		val |= (0xff << (8 - cnt)) & (uae_u32)-sign;
		val &= 0xff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSR.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e028_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 8) {
		SET_CFLG ((cnt == 8) & (val >> 7));
		COPY_CARRY ();
		val = 0;
	} else if (cnt > 0) {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXR.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e030_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 36) cnt -= 36;
	if (cnt >= 18) cnt -= 18;
	if (cnt >= 9) cnt -= 9;
	if (cnt > 0) {
	cnt--;
	{
	uae_u32 carry;
	uae_u32 hival = (val << 1) | GET_XFLG ();
	hival <<= (7 - cnt);
	val >>= cnt;
	carry = val & 1;
	val >>= 1;
	val |= hival;
	SET_XFLG (carry);
	val &= 0xff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

#endif

#ifdef PART_8
/* ROR.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e038_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt > 0) {	uae_u32 hival;
	cnt &= 7;
	hival = val << (8 - cnt);
	val >>= cnt;
	val |= hival;
	val &= 0xff;
	SET_CFLG ((val & 0x80) >> 7);
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASRQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e040_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = (0x8000 & val) >> 15;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 16) {
		val = 0xffff & (uae_u32)-sign;
		SET_CFLG (sign);
		COPY_CARRY ();
	} else {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
		val |= (0xffff << (16 - cnt)) & (uae_u32)-sign;
		val &= 0xffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSRQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e048_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 16) {
		SET_CFLG ((cnt == 16) & (val >> 15));
		COPY_CARRY ();
		val = 0;
	} else {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXRQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e050_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
{	cnt--;
	{
	uae_u32 carry;
	uae_u32 hival = (val << 1) | GET_XFLG ();
	hival <<= (15 - cnt);
	val >>= cnt;
	carry = val & 1;
	val >>= 1;
	val |= hival;
	SET_XFLG (carry);
	val &= 0xffff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* RORQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e058_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
{	uae_u32 hival;
	cnt &= 15;
	hival = val << (16 - cnt);
	val >>= cnt;
	val |= hival;
	val &= 0xffff;
	SET_CFLG ((val & 0x8000) >> 15);
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASR.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e060_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = (0x8000 & val) >> 15;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 16) {
		val = 0xffff & (uae_u32)-sign;
		SET_CFLG (sign);
		COPY_CARRY ();
	} else if (cnt > 0) {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
		val |= (0xffff << (16 - cnt)) & (uae_u32)-sign;
		val &= 0xffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSR.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e068_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 16) {
		SET_CFLG ((cnt == 16) & (val >> 15));
		COPY_CARRY ();
		val = 0;
	} else if (cnt > 0) {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXR.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e070_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 34) cnt -= 34;
	if (cnt >= 17) cnt -= 17;
	if (cnt > 0) {
	cnt--;
	{
	uae_u32 carry;
	uae_u32 hival = (val << 1) | GET_XFLG ();
	hival <<= (15 - cnt);
	val >>= cnt;
	carry = val & 1;
	val >>= 1;
	val |= hival;
	SET_XFLG (carry);
	val &= 0xffff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROR.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e078_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt > 0) {	uae_u32 hival;
	cnt &= 15;
	hival = val << (16 - cnt);
	val >>= cnt;
	val |= hival;
	val &= 0xffff;
	SET_CFLG ((val & 0x8000) >> 15);
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASRQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e080_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	uae_u32 sign = (0x80000000 & val) >> 31;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 32) {
		val = 0xffffffff & (uae_u32)-sign;
		SET_CFLG (sign);
		COPY_CARRY ();
	} else {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
		val |= (0xffffffff << (32 - cnt)) & (uae_u32)-sign;
		val &= 0xffffffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSRQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e088_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 32) {
		SET_CFLG ((cnt == 32) & (val >> 31));
		COPY_CARRY ();
		val = 0;
	} else {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXRQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e090_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
{	cnt--;
	{
	uae_u32 carry;
	uae_u32 hival = (val << 1) | GET_XFLG ();
	hival <<= (31 - cnt);
	val >>= cnt;
	carry = val & 1;
	val >>= 1;
	val |= hival;
	SET_XFLG (carry);
	val &= 0xffffffff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* RORQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e098_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
{	uae_u32 hival;
	cnt &= 31;
	hival = val << (32 - cnt);
	val >>= cnt;
	val |= hival;
	val &= 0xffffffff;
	SET_CFLG ((val & 0x80000000) >> 31);
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASR.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e0a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	uae_u32 sign = (0x80000000 & val) >> 31;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 32) {
		val = 0xffffffff & (uae_u32)-sign;
		SET_CFLG (sign);
		COPY_CARRY ();
	} else if (cnt > 0) {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
		val |= (0xffffffff << (32 - cnt)) & (uae_u32)-sign;
		val &= 0xffffffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSR.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e0a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 32) {
		SET_CFLG ((cnt == 32) & (val >> 31));
		COPY_CARRY ();
		val = 0;
	} else if (cnt > 0) {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXR.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e0b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 33) cnt -= 33;
	if (cnt > 0) {
	cnt--;
	{
	uae_u32 carry;
	uae_u32 hival = (val << 1) | GET_XFLG ();
	hival <<= (31 - cnt);
	val >>= cnt;
	carry = val & 1;
	val >>= 1;
	val |= hival;
	SET_XFLG (carry);
	val &= 0xffffffff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROR.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e0b8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt > 0) {	uae_u32 hival;
	cnt &= 31;
	hival = val << (32 - cnt);
	val >>= cnt;
	val |= hival;
	val &= 0xffffffff;
	SET_CFLG ((val & 0x80000000) >> 31);
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASRW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e0d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ASRW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e0d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ASRW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e0e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = dataa;
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ASRW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e0e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ASRW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e0f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ASRW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e0f8_33)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ASRW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e0f9_33)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ASLQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e100_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 8) {
		SET_VFLG (val != 0);
		SET_CFLG (cnt == 8 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else {
		uae_u32 mask = (0xff << (7 - cnt)) & 0xff;
		SET_VFLG ((val & mask) != mask && (val & mask) != 0);
		val <<= cnt - 1;
		SET_CFLG ((val & 0x80) >> 7);
		COPY_CARRY ();
		val <<= 1;
		val &= 0xff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSLQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e108_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 8) {
		SET_CFLG (cnt == 8 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else {
		val <<= (cnt - 1);
		SET_CFLG ((val & 0x80) >> 7);
		COPY_CARRY ();
		val <<= 1;
	val &= 0xff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXLQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e110_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
{	cnt--;
	{
	uae_u32 carry;
	uae_u32 loval = val >> (7 - cnt);
	carry = loval & 1;
	val = (((val << 1) | GET_XFLG ()) << cnt) | (loval >> 1);
	SET_XFLG (carry);
	val &= 0xff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROLQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e118_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
{	uae_u32 loval;
	cnt &= 7;
	loval = val >> (8 - cnt);
	val <<= cnt;
	val |= loval;
	val &= 0xff;
	SET_CFLG (val & 1);
}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASL.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e120_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 8) {
		SET_VFLG (val != 0);
		SET_CFLG (cnt == 8 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else if (cnt > 0) {
		uae_u32 mask = (0xff << (7 - cnt)) & 0xff;
		SET_VFLG ((val & mask) != mask && (val & mask) != 0);
		val <<= cnt - 1;
		SET_CFLG ((val & 0x80) >> 7);
		COPY_CARRY ();
		val <<= 1;
		val &= 0xff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSL.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e128_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 8) {
		SET_CFLG (cnt == 8 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else if (cnt > 0) {
		val <<= (cnt - 1);
		SET_CFLG ((val & 0x80) >> 7);
		COPY_CARRY ();
		val <<= 1;
	val &= 0xff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXL.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e130_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 36) cnt -= 36;
	if (cnt >= 18) cnt -= 18;
	if (cnt >= 9) cnt -= 9;
	if (cnt > 0) {
	cnt--;
	{
	uae_u32 carry;
	uae_u32 loval = val >> (7 - cnt);
	carry = loval & 1;
	val = (((val << 1) | GET_XFLG ()) << cnt) | (loval >> 1);
	SET_XFLG (carry);
	val &= 0xff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROL.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e138_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt > 0) {
	uae_u32 loval;
	cnt &= 7;
	loval = val >> (8 - cnt);
	val <<= cnt;
	val |= loval;
	val &= 0xff;
	SET_CFLG (val & 1);
}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASLQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e140_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 16) {
		SET_VFLG (val != 0);
		SET_CFLG (cnt == 16 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else {
		uae_u32 mask = (0xffff << (15 - cnt)) & 0xffff;
		SET_VFLG ((val & mask) != mask && (val & mask) != 0);
		val <<= cnt - 1;
		SET_CFLG ((val & 0x8000) >> 15);
		COPY_CARRY ();
		val <<= 1;
		val &= 0xffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSLQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e148_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 16) {
		SET_CFLG (cnt == 16 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else {
		val <<= (cnt - 1);
		SET_CFLG ((val & 0x8000) >> 15);
		COPY_CARRY ();
		val <<= 1;
	val &= 0xffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXLQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e150_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
{	cnt--;
	{
	uae_u32 carry;
	uae_u32 loval = val >> (15 - cnt);
	carry = loval & 1;
	val = (((val << 1) | GET_XFLG ()) << cnt) | (loval >> 1);
	SET_XFLG (carry);
	val &= 0xffff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROLQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e158_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
{	uae_u32 loval;
	cnt &= 15;
	loval = val >> (16 - cnt);
	val <<= cnt;
	val |= loval;
	val &= 0xffff;
	SET_CFLG (val & 1);
}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASL.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e160_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 16) {
		SET_VFLG (val != 0);
		SET_CFLG (cnt == 16 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else if (cnt > 0) {
		uae_u32 mask = (0xffff << (15 - cnt)) & 0xffff;
		SET_VFLG ((val & mask) != mask && (val & mask) != 0);
		val <<= cnt - 1;
		SET_CFLG ((val & 0x8000) >> 15);
		COPY_CARRY ();
		val <<= 1;
		val &= 0xffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSL.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e168_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 16) {
		SET_CFLG (cnt == 16 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else if (cnt > 0) {
		val <<= (cnt - 1);
		SET_CFLG ((val & 0x8000) >> 15);
		COPY_CARRY ();
		val <<= 1;
	val &= 0xffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXL.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e170_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 34) cnt -= 34;
	if (cnt >= 17) cnt -= 17;
	if (cnt > 0) {
	cnt--;
	{
	uae_u32 carry;
	uae_u32 loval = val >> (15 - cnt);
	carry = loval & 1;
	val = (((val << 1) | GET_XFLG ()) << cnt) | (loval >> 1);
	SET_XFLG (carry);
	val &= 0xffff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROL.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e178_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt > 0) {
	uae_u32 loval;
	cnt &= 15;
	loval = val >> (16 - cnt);
	val <<= cnt;
	val |= loval;
	val &= 0xffff;
	SET_CFLG (val & 1);
}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASLQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e180_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 32) {
		SET_VFLG (val != 0);
		SET_CFLG (cnt == 32 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else {
		uae_u32 mask = (0xffffffff << (31 - cnt)) & 0xffffffff;
		SET_VFLG ((val & mask) != mask && (val & mask) != 0);
		val <<= cnt - 1;
		SET_CFLG ((val & 0x80000000) >> 31);
		COPY_CARRY ();
		val <<= 1;
		val &= 0xffffffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSLQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e188_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 32) {
		SET_CFLG (cnt == 32 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else {
		val <<= (cnt - 1);
		SET_CFLG ((val & 0x80000000) >> 31);
		COPY_CARRY ();
		val <<= 1;
	val &= 0xffffffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXLQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e190_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
{	cnt--;
	{
	uae_u32 carry;
	uae_u32 loval = val >> (31 - cnt);
	carry = loval & 1;
	val = (((val << 1) | GET_XFLG ()) << cnt) | (loval >> 1);
	SET_XFLG (carry);
	val &= 0xffffffff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROLQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e198_33)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
{	uae_u32 loval;
	cnt &= 31;
	loval = val >> (32 - cnt);
	val <<= cnt;
	val |= loval;
	val &= 0xffffffff;
	SET_CFLG (val & 1);
}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASL.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e1a0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 32) {
		SET_VFLG (val != 0);
		SET_CFLG (cnt == 32 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else if (cnt > 0) {
		uae_u32 mask = (0xffffffff << (31 - cnt)) & 0xffffffff;
		SET_VFLG ((val & mask) != mask && (val & mask) != 0);
		val <<= cnt - 1;
		SET_CFLG ((val & 0x80000000) >> 31);
		COPY_CARRY ();
		val <<= 1;
		val &= 0xffffffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSL.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e1a8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 32) {
		SET_CFLG (cnt == 32 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else if (cnt > 0) {
		val <<= (cnt - 1);
		SET_CFLG ((val & 0x80000000) >> 31);
		COPY_CARRY ();
		val <<= 1;
	val &= 0xffffffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXL.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e1b0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 33) cnt -= 33;
	if (cnt > 0) {
	cnt--;
	{
	uae_u32 carry;
	uae_u32 loval = val >> (31 - cnt);
	carry = loval & 1;
	val = (((val << 1) | GET_XFLG ()) << cnt) | (loval >> 1);
	SET_XFLG (carry);
	val &= 0xffffffff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROL.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e1b8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt > 0) {
	uae_u32 loval;
	cnt &= 31;
	loval = val >> (32 - cnt);
	val <<= cnt;
	val |= loval;
	val &= 0xffffffff;
	SET_CFLG (val & 1);
}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASLW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e1d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ASLW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e1d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ASLW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e1e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = dataa;
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ASLW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e1e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ASLW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e1f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	put_rmw_word_mmu060 (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ASLW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e1f8_33)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ASLW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e1f9_33)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* LSRW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e2d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* LSRW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e2d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* LSRW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e2e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = dataa;
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* LSRW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e2e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* LSRW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e2f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* LSRW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e2f8_33)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* LSRW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e2f9_33)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* LSLW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e3d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* LSLW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e3d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* LSLW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e3e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = dataa;
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* LSLW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e3e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* LSLW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e3f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* LSLW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e3f8_33)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* LSLW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e3f9_33)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ROXRW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e4d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ROXRW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e4d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ROXRW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e4e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = dataa;
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ROXRW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e4e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ROXRW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e4f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ROXRW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e4f8_33)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ROXRW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e4f9_33)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ROXLW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e5d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ROXLW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e5d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ROXLW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e5e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = dataa;
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ROXLW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e5e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ROXLW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e5f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ROXLW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e5f8_33)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ROXLW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e5f9_33)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* RORW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e6d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* RORW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e6d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* RORW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e6e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = dataa;
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* RORW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e6e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* RORW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e6f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	put_rmw_word_mmu060 (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* RORW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e6f8_33)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* RORW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e6f9_33)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ROLW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e7d0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ROLW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e7d8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ROLW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e7e0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = dataa;
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ROLW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e7e8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ROLW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e7f0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	put_rmw_word_mmu060 (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ROLW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e7f8_33)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ROLW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e7f9_33)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu060 (2);
{	uae_s16 data = get_rmw_word_mmu060 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	put_rmw_word_mmu060 (dataa, val);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BFTST.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e8c0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{{	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp = m68k_dreg(regs, dstreg);
	offset &= 0x1f;
	tmp = (tmp << offset) | (tmp >> (32 - offset));
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFTST.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e8d0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFTST.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e8e8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFTST.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e8f0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFTST.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e8f8_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFTST.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e8f9_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
}}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* BFTST.L #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e8fa_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFTST.L #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e8fb_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTU.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e9c0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{{	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp = m68k_dreg(regs, dstreg);
	offset &= 0x1f;
	tmp = (tmp << offset) | (tmp >> (32 - offset));
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTU.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e9d0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTU.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e9e8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTU.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e9f0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTU.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e9f8_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTU.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e9f9_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTU.L #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e9fa_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTU.L #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e9fb_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFCHG.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eac0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp = m68k_dreg(regs, dstreg);
	offset &= 0x1f;
	tmp = (tmp << offset) | (tmp >> (32 - offset));
	bdata[0] = tmp & ((1 << (32 - width)) - 1);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = tmp ^ (0xffffffffu >> (32 - width));
	tmp = bdata[0] | (tmp << (32 - width));
	m68k_dreg(regs, dstreg) = (tmp >> offset) | (tmp << (32 - offset));
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFCHG.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ead0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = mmu060_get_rmw_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = tmp ^ (0xffffffffu >> (32 - width));
	mmu060_put_rmw_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFCHG.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eae8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = mmu060_get_rmw_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = tmp ^ (0xffffffffu >> (32 - width));
	mmu060_put_rmw_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFCHG.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eaf0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = mmu060_get_rmw_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = tmp ^ (0xffffffffu >> (32 - width));
	mmu060_put_rmw_bitfield(dsta, bdata, tmp, offset, width);
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFCHG.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eaf8_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = mmu060_get_rmw_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = tmp ^ (0xffffffffu >> (32 - width));
	mmu060_put_rmw_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFCHG.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eaf9_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = mmu060_get_rmw_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = tmp ^ (0xffffffffu >> (32 - width));
	mmu060_put_rmw_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTS.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ebc0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{{	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp = m68k_dreg(regs, dstreg);
	offset &= 0x1f;
	tmp = (tmp << offset) | (tmp >> (32 - offset));
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp = (uae_s32)tmp >> (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTS.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ebd0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp = (uae_s32)tmp >> (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTS.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ebe8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp = (uae_s32)tmp >> (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTS.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ebf0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp = (uae_s32)tmp >> (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTS.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ebf8_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp = (uae_s32)tmp >> (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTS.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ebf9_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp = (uae_s32)tmp >> (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTS.L #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ebfa_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp = (uae_s32)tmp >> (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTS.L #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ebfb_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp = (uae_s32)tmp >> (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFCLR.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ecc0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp = m68k_dreg(regs, dstreg);
	offset &= 0x1f;
	tmp = (tmp << offset) | (tmp >> (32 - offset));
	bdata[0] = tmp & ((1 << (32 - width)) - 1);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0;
	tmp = bdata[0] | (tmp << (32 - width));
	m68k_dreg(regs, dstreg) = (tmp >> offset) | (tmp << (32 - offset));
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFCLR.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ecd0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = mmu060_get_rmw_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0;
	mmu060_put_rmw_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFCLR.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ece8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = mmu060_get_rmw_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0;
	mmu060_put_rmw_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFCLR.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ecf0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = mmu060_get_rmw_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0;
	mmu060_put_rmw_bitfield(dsta, bdata, tmp, offset, width);
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFCLR.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ecf8_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = mmu060_get_rmw_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0;
	mmu060_put_rmw_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFCLR.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ecf9_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = mmu060_get_rmw_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0;
	mmu060_put_rmw_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* BFFFO.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_edc0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{{	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp = m68k_dreg(regs, dstreg);
	offset &= 0x1f;
	tmp = (tmp << offset) | (tmp >> (32 - offset));
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	{ uae_u32 mask = 1 << (width - 1);
	while (mask) { if (tmp & mask) break; mask >>= 1; offset++; }}
	m68k_dreg (regs, (extra >> 12) & 7) = offset;
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFFFO.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_edd0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	{ uae_u32 mask = 1 << (width - 1);
	while (mask) { if (tmp & mask) break; mask >>= 1; offset++; }}
	m68k_dreg (regs, (extra >> 12) & 7) = offset;
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFFFO.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ede8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	{ uae_u32 mask = 1 << (width - 1);
	while (mask) { if (tmp & mask) break; mask >>= 1; offset++; }}
	m68k_dreg (regs, (extra >> 12) & 7) = offset;
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFFFO.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_edf0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	{ uae_u32 mask = 1 << (width - 1);
	while (mask) { if (tmp & mask) break; mask >>= 1; offset++; }}
	m68k_dreg (regs, (extra >> 12) & 7) = offset;
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFFFO.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_edf8_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	{ uae_u32 mask = 1 << (width - 1);
	while (mask) { if (tmp & mask) break; mask >>= 1; offset++; }}
	m68k_dreg (regs, (extra >> 12) & 7) = offset;
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFFFO.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_edf9_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	{ uae_u32 mask = 1 << (width - 1);
	while (mask) { if (tmp & mask) break; mask >>= 1; offset++; }}
	m68k_dreg (regs, (extra >> 12) & 7) = offset;
}}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* BFFFO.L #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_edfa_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	{ uae_u32 mask = 1 << (width - 1);
	while (mask) { if (tmp & mask) break; mask >>= 1; offset++; }}
	m68k_dreg (regs, (extra >> 12) & 7) = offset;
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFFFO.L #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_edfb_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	{ uae_u32 mask = 1 << (width - 1);
	while (mask) { if (tmp & mask) break; mask >>= 1; offset++; }}
	m68k_dreg (regs, (extra >> 12) & 7) = offset;
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFSET.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eec0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp = m68k_dreg(regs, dstreg);
	offset &= 0x1f;
	tmp = (tmp << offset) | (tmp >> (32 - offset));
	bdata[0] = tmp & ((1 << (32 - width)) - 1);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0xffffffffu >> (32 - width);
	tmp = bdata[0] | (tmp << (32 - width));
	m68k_dreg(regs, dstreg) = (tmp >> offset) | (tmp << (32 - offset));
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFSET.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eed0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = mmu060_get_rmw_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0xffffffffu >> (32 - width);
	mmu060_put_rmw_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFSET.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eee8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = mmu060_get_rmw_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0xffffffffu >> (32 - width);
	mmu060_put_rmw_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFSET.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eef0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = mmu060_get_rmw_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0xffffffffu >> (32 - width);
	mmu060_put_rmw_bitfield(dsta, bdata, tmp, offset, width);
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFSET.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eef8_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = mmu060_get_rmw_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0xffffffffu >> (32 - width);
	mmu060_put_rmw_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFSET.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eef9_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = mmu060_get_rmw_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0xffffffffu >> (32 - width);
	mmu060_put_rmw_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* BFINS.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_efc0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp = m68k_dreg(regs, dstreg);
	offset &= 0x1f;
	tmp = (tmp << offset) | (tmp >> (32 - offset));
	bdata[0] = tmp & ((1 << (32 - width)) - 1);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = m68k_dreg (regs, (extra >> 12) & 7);
	tmp = tmp & (0xffffffffu >> (32 - width));
	SET_NFLG (tmp & (1 << (width - 1)) ? 1 : 0);
	SET_ZFLG (tmp == 0);
	tmp = bdata[0] | (tmp << (32 - width));
	m68k_dreg(regs, dstreg) = (tmp >> offset) | (tmp << (32 - offset));
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFINS.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_efd0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = mmu060_get_rmw_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = m68k_dreg (regs, (extra >> 12) & 7);
	tmp = tmp & (0xffffffffu >> (32 - width));
	SET_NFLG (tmp & (1 << (width - 1)) ? 1 : 0);
	SET_ZFLG (tmp == 0);
	mmu060_put_rmw_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFINS.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_efe8_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = mmu060_get_rmw_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = m68k_dreg (regs, (extra >> 12) & 7);
	tmp = tmp & (0xffffffffu >> (32 - width));
	SET_NFLG (tmp & (1 << (width - 1)) ? 1 : 0);
	SET_ZFLG (tmp == 0);
	mmu060_put_rmw_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFINS.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eff0_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = mmu060_get_rmw_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = m68k_dreg (regs, (extra >> 12) & 7);
	tmp = tmp & (0xffffffffu >> (32 - width));
	SET_NFLG (tmp & (1 << (width - 1)) ? 1 : 0);
	SET_ZFLG (tmp == 0);
	mmu060_put_rmw_bitfield(dsta, bdata, tmp, offset, width);
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFINS.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eff8_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = mmu060_get_rmw_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = m68k_dreg (regs, (extra >> 12) & 7);
	tmp = tmp & (0xffffffffu >> (32 - width));
	SET_NFLG (tmp & (1 << (width - 1)) ? 1 : 0);
	SET_ZFLG (tmp == 0);
	mmu060_put_rmw_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFINS.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eff9_33)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu060 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu060 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = mmu060_get_rmw_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = m68k_dreg (regs, (extra >> 12) & 7);
	tmp = tmp & (0xffffffffu >> (32 - width));
	SET_NFLG (tmp & (1 << (width - 1)) ? 1 : 0);
	SET_ZFLG (tmp == 0);
	mmu060_put_rmw_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L Dn,#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f000_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16397; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	uae_u16 extraa = 0;
	mmu_op30 (pc, opcode, extra, extraa);
}}endlabel16397: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L An,#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f008_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16398; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	uae_u16 extraa = 0;
	mmu_op30 (pc, opcode, extra, extraa);
}}endlabel16398: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L (An),#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f010_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16399; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
{	uaecptr extraa;
	extraa = m68k_areg (regs, srcreg);
	mmu_op30 (pc, opcode, extra, extraa);
}}}endlabel16399: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L (An)+,#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f018_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16400; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
{	uaecptr extraa;
	extraa = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
	mmu_op30 (pc, opcode, extra, extraa);
}}}endlabel16400: ;
	mmufixup[0].reg = -1;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L -(An),#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f020_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16401; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
{	uaecptr extraa;
	extraa = m68k_areg (regs, srcreg) - 4;
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = extraa;
	mmu_op30 (pc, opcode, extra, extraa);
}}}endlabel16401: ;
	mmufixup[0].reg = -1;
return 6 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L (d16,An),#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f028_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16402; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
{	uaecptr extraa;
	extraa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu060 (0);
	m68k_incpci (2);
	mmu_op30 (pc, opcode, extra, extraa);
}}}endlabel16402: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L (d8,An,Xn),#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f030_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16403; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
{	uaecptr extraa;
{	extraa = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
	mmu_op30 (pc, opcode, extra, extraa);
}}}}endlabel16403: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L (xxx).W,#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f038_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel16404; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
{	uaecptr extraa;
	extraa = (uae_s32)(uae_s16)get_iword_mmu060 (0);
	m68k_incpci (2);
	mmu_op30 (pc, opcode, extra, extraa);
}}}endlabel16404: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L (xxx).L,#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f039_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel16405; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
{	uaecptr extraa;
	extraa = get_ilong_mmu060 (0);
	m68k_incpci (4);
	mmu_op30 (pc, opcode, extra, extraa);
}}}endlabel16405: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f200_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,An */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f208_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f210_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f218_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f220_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f228_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f230_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f238_33)(uae_u32 opcode)
{
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f239_33)(uae_u32 opcode)
{
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f23a_33)(uae_u32 opcode)
{
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f23b_33)(uae_u32 opcode)
{
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,#<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f23c_33)(uae_u32 opcode)
{
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f240_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FDBcc.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f248_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_dbcc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f250_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f258_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f260_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f268_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f270_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f278_33)(uae_u32 opcode)
{
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f279_33)(uae_u32 opcode)
{
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FTRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f27a_33)(uae_u32 opcode)
{
{
#ifdef FPUEMU
	uaecptr oldpc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu060 (2);
{	uae_s16 dummy = get_iword_mmu060 (4);
	m68k_incpci (6);
	fpuop_trapcc (opcode, oldpc, extra);
}
#endif
}return 12 * CYCLE_UNIT / 2;
}

#endif
/* FTRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f27b_33)(uae_u32 opcode)
{
{
#ifdef FPUEMU
	uaecptr oldpc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu060 (2);
{	uae_s32 dummy;
	dummy = get_ilong_mmu060 (4);
	m68k_incpci (8);
	fpuop_trapcc (opcode, oldpc, extra);
}
#endif
}return 16 * CYCLE_UNIT / 2;
}

#endif
/* FTRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f27c_33)(uae_u32 opcode)
{
{
#ifdef FPUEMU
	uaecptr oldpc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu060 (2);
	m68k_incpci (4);
	fpuop_trapcc (opcode, oldpc, extra);

#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FBccQ.L #<data>,#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f280_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 63);
{
#ifdef FPUEMU
	m68k_incpci (2);
{	uaecptr pc = m68k_getpc ();
{	uae_s16 extra = get_iword_mmu060 (0);
	m68k_incpci (2);
	fpuop_bcc (opcode, pc,extra);
}}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FBccQ.L #<data>,#<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f2c0_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 63);
{
#ifdef FPUEMU
	m68k_incpci (2);
{	uaecptr pc = m68k_getpc ();
{	uae_s32 extra;
	extra = get_ilong_mmu060 (0);
	m68k_incpci (4);
	fpuop_bcc (opcode, pc,extra);
}}
#endif
}return 12 * CYCLE_UNIT / 2;
}

#endif
/* FSAVE.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f310_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16432; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_save (opcode);

#endif
}}endlabel16432: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FSAVE.L -(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f320_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16433; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_save (opcode);

#endif
}}endlabel16433: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FSAVE.L (d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f328_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16434; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_save (opcode);

#endif
}}endlabel16434: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FSAVE.L (d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f330_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16435; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_save (opcode);

#endif
}}endlabel16435: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FSAVE.L (xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f338_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel16436; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_save (opcode);

#endif
}}endlabel16436: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FSAVE.L (xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f339_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel16437; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_save (opcode);

#endif
}}endlabel16437: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f350_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16438; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel16438: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f358_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16439; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel16439: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f368_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16440; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel16440: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f370_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16441; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel16441: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f378_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel16442; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel16442: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f379_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel16443; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel16443: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f37a_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel16444; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel16444: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f37b_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel16445; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel16445: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CINVLQ.L #<data>,An */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f408_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel16446; }
{	flush_mmu060(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel16446: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CINVPQ.L #<data>,An */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f410_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel16447; }
{	flush_mmu060(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel16447: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CINVAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f418_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel16448; }
{	flush_mmu060(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel16448: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CINVAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f419_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel16449; }
{	flush_mmu060(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel16449: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CINVAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f41a_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel16450; }
{	flush_mmu060(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel16450: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CINVAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f41b_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel16451; }
{	flush_mmu060(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel16451: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CINVAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f41c_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel16452; }
{	flush_mmu060(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel16452: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CINVAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f41d_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel16453; }
{	flush_mmu060(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel16453: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CINVAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f41e_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel16454; }
{	flush_mmu060(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel16454: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CINVAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f41f_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel16455; }
{	flush_mmu060(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel16455: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CPUSHLQ.L #<data>,An */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f428_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel16456; }
{	flush_mmu060(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel16456: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CPUSHPQ.L #<data>,An */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f430_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel16457; }
{	flush_mmu060(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel16457: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CPUSHAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f438_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel16458; }
{	flush_mmu060(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel16458: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CPUSHAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f439_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel16459; }
{	flush_mmu060(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel16459: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CPUSHAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f43a_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel16460; }
{	flush_mmu060(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel16460: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CPUSHAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f43b_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel16461; }
{	flush_mmu060(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel16461: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CPUSHAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f43c_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel16462; }
{	flush_mmu060(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel16462: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CPUSHAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f43d_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel16463; }
{	flush_mmu060(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel16463: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CPUSHAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f43e_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel16464; }
{	flush_mmu060(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel16464: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CPUSHAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f43f_33)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel16465; }
{	flush_mmu060(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel16465: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* PFLUSHN.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f500_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16466; }
{	m68k_incpci (2);
	mmu_op (opcode, 0);
}}endlabel16466: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* PFLUSH.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f508_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16467; }
{	m68k_incpci (2);
	mmu_op (opcode, 0);
}}endlabel16467: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* PFLUSHAN.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f510_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16468; }
{	m68k_incpci (2);
	mmu_op (opcode, 0);
}}endlabel16468: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* PFLUSHA.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f518_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16469; }
{	m68k_incpci (2);
	mmu_op (opcode, 0);
}}endlabel16469: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* PTESTW.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f548_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16470; }
{	m68k_incpci (2);
	mmu_op (opcode, 0);
}}endlabel16470: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* PTESTR.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f568_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16471; }
{	m68k_incpci (2);
	mmu_op (opcode, 0);
}}endlabel16471: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* PLPAW.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f588_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16472; }
{	m68k_incpci (2);
	mmu_op (opcode, 0);
}}endlabel16472: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* PLPAR.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f5c8_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel16473; }
{	m68k_incpci (2);
	mmu_op (opcode, 0);
}}endlabel16473: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* MOVE16.L (An)+,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f600_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	uae_u32 v[4];
{	uaecptr memsa;
	memsa = m68k_areg (regs, srcreg);
{	uaecptr memda;
	memda = get_ilong_mmu060 (2);
	memsa &= ~15;
	memda &= ~15;
	get_move16_mmu (memsa, v);
	put_move16_mmu (memda, v);
	m68k_areg (regs, srcreg) += 16;
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* MOVE16.L (xxx).L,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f608_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u32 v[4];
{	uaecptr memsa;
	memsa = get_ilong_mmu060 (2);
{	uaecptr memda;
	memda = m68k_areg (regs, dstreg);
	memsa &= ~15;
	memda &= ~15;
	get_move16_mmu (memsa, v);
	put_move16_mmu (memda, v);
	m68k_areg (regs, dstreg) += 16;
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* MOVE16.L (An),(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f610_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	uae_u32 v[4];
{	uaecptr memsa;
	memsa = m68k_areg (regs, srcreg);
{	uaecptr memda;
	memda = get_ilong_mmu060 (2);
	memsa &= ~15;
	memda &= ~15;
	get_move16_mmu (memsa, v);
	put_move16_mmu (memda, v);
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* MOVE16.L (xxx).L,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f618_33)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u32 v[4];
{	uaecptr memsa;
	memsa = get_ilong_mmu060 (2);
{	uaecptr memda;
	memda = m68k_areg (regs, dstreg);
	memsa &= ~15;
	memda &= ~15;
	get_move16_mmu (memsa, v);
	put_move16_mmu (memda, v);
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* MOVE16.L (An)+,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f620_33)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = 0;
{	uae_u32 v[4];
	uaecptr mems = m68k_areg (regs, srcreg) & ~15, memd;
	dstreg = (get_iword_mmu060 (2) >> 12) & 7;
	memd = m68k_areg (regs, dstreg) & ~15;
	get_move16_mmu (mems, v);
	put_move16_mmu (memd, v);
	if (srcreg != dstreg)
		m68k_areg (regs, srcreg) += 16;
	m68k_areg (regs, dstreg) += 16;
}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* LPSTOP.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f800_33)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel16479; }
{	uae_u16 sw = x_get_iword (2);
	uae_u16 sr;
	if (sw != (0x100|0x80|0x40)) { Exception (4); goto endlabel16479; }
	sr = x_get_iword (4);
	if (!(sr & 0x8000)) { Exception (8); goto endlabel16479; }
	regs.sr = sr;
	MakeFromSR ();
	m68k_setstopped();
	m68k_incpci (6);
}}endlabel16479: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
#endif

