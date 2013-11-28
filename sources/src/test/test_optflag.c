 /*
  * E-UAE - The portable Amiga Emulator
  *
  * Test suite for optflag_* functions.
  *
  * Copyright Richard Drummond 2007
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include <stdio.h>
#include <stdlib.h>

#include "options.h"
#include "events.h"
#include "uae.h"
#include "memory_uae.h"
#include "custom.h"
#include "newcpu.h"

struct regstruct regs;

#include "machdep/m68kops.h"

int num_fails;
int num_tests;

static void print_flags (int x, int n, int z, int v, int c)
{
    putc (x ? 'X' : '-', stdout);
    putc (n ? 'N' : '-', stdout);
    putc (z ? 'Z' : '-', stdout);
    putc (v ? 'V' : '-', stdout);
    putc (c ? 'C' : '-', stdout);
}

static uae_u32 test_test (uae_u32 dst, int size, uae_u32 x)
{
    uae_u32 Dm;
    uae_u32 n, z, v, c;

    num_tests++;

    Dm  = (dst >> (size - 1)) & 1;

    /* Expected flags. See, for example, M68000 Programmer's Reference Manual, Appendix A. */
    n   = Dm;
    z   = (dst == 0) ? 1 : 0;
    v   = 0;
    c   = 0;

    /* Set initial flags to opposite of what we expect - just to make sure they are modified. */
    SET_NFLG (&regs.ccrflags, n ^ 1);
    SET_ZFLG (&regs.ccrflags, z ^ 1);
    SET_VFLG (&regs.ccrflags, v ^ 1);
    SET_CFLG (&regs.ccrflags, c ^ 1);
    SET_XFLG (&regs.ccrflags, x);     /* X should be unchanged. */

    if (size == 8)
	optflag_testb (&regs, (uae_s8) dst);
    else if (size == 16)
	optflag_testw (&regs, (uae_s16) dst);
    else
	optflag_testl (&regs, (uae_s32) dst);

    /* Check the result */
    if (GET_XFLG(&regs.ccrflags) != x || GET_NFLG(&regs.ccrflags) != n || GET_ZFLG(&regs.ccrflags) != z ||
	GET_VFLG(&regs.ccrflags) != v || GET_CFLG(&regs.ccrflags) != c) {
	printf ("TEST.%c(%08x) failed. ", size == 8 ? 'B' : size == 16 ? 'W' : 'L', dst);
	printf ("Expected flags=");
	print_flags (x, n, z, v, c);
	printf ("; got flags=");
	print_flags (GET_XFLG(&regs.ccrflags), GET_NFLG(&regs.ccrflags), GET_ZFLG(&regs.ccrflags),
		     GET_VFLG(&regs.ccrflags), GET_CFLG(&regs.ccrflags));
	printf (".\n");

	num_fails++;
    }
    return 0;
}

static uae_u32 test_add (uae_u32 src, uae_u32 dst, int size)
{
    uae_u32 res, res2;
    uae_u32 Sm, Dm, Rm;
    uae_u32 x, n, z, v, c;

    num_tests++;

    /* Expected result. */
    res = dst + src;
    if (size == 16)
	res &= 0xFFFF;
    else if (size==8)
	res &= 0xFF;

    Sm  = (src >> (size - 1)) & 1;
    Dm  = (dst >> (size - 1)) & 1;
    Rm  = (res >> (size - 1)) & 1;

    /* Expected flags. See, for example, M68000 Programmer's Reference Manual, Appendix A. */
    n   = Rm;
    z   = (res == 0) ? 1 : 0;
    v   = (Sm & Dm & !Rm) | (!Sm & !Dm & Rm);
    c   = (Sm & Dm) | (!Rm & Dm) | (Sm & !Rm);
    x   = c;

    /* Set initial flags to opposite of what we expect - just to make sure they are modified. */
    SET_NFLG (&regs.ccrflags, n ^ 1);
    SET_ZFLG (&regs.ccrflags, z ^ 1);
    SET_VFLG (&regs.ccrflags, v ^ 1);
    SET_CFLG (&regs.ccrflags, c ^ 1);
    SET_XFLG (&regs.ccrflags, x ^ 1);

    if (size == 8) {
	optflag_addb (&regs, res2, (uae_s8) src, (uae_s8) dst);
	res2 &= 0xFF;
    } else if (size == 16) {
	optflag_addw (&regs, res2, (uae_s16) src, (uae_s16) dst);
	res2 &= 0xFFFF;
    } else
	optflag_addl (&regs, res2, (uae_s32) src, (uae_s32) dst);

    /* Check the result */
    if (res2 != res ||  GET_XFLG(&regs.ccrflags) != x || GET_NFLG(&regs.ccrflags) != n || GET_ZFLG(&regs.ccrflags) != z ||
			GET_VFLG(&regs.ccrflags) != v || GET_CFLG(&regs.ccrflags) != c) {
	printf ("ADD.%c(%08x,%08x) failed. ", size == 8 ? 'B' : size == 16 ? 'W' : 'L', dst, src);
	printf ("Expected=%08x; got=%08x. Expected flags=", res, res2);
	print_flags (x, n, z, v, c);
	printf ("; got flags=");
	print_flags (GET_XFLG(&regs.ccrflags), GET_NFLG(&regs.ccrflags), GET_ZFLG(&regs.ccrflags),
		     GET_VFLG(&regs.ccrflags), GET_CFLG(&regs.ccrflags));
	printf (".\n");

	num_fails++;
    }
    return res2;
}

static uae_u32 test_sub (uae_u32 src, uae_u32 dst, int size)
{
    uae_u32 res, res2;
    uae_u32 Sm, Dm, Rm;
    uae_u32 x, n, z, v, c;

    num_tests++;

    /* Expected result. */
    res = dst - src;
    if (size == 16)
	res &= 0xFFFF;
    else if (size==8)
	res &= 0xFF;

    Sm  = (src >> (size - 1)) & 1;
    Dm  = (dst >> (size - 1)) & 1;
    Rm  = (res >> (size - 1)) & 1;

    /* Expected flags. See, for example, M68000 Programmer's Reference Manual, Appendix A. */
    n   = Rm;
    z   = (res == 0) ? 1 : 0;
    v   = (!Sm & Dm & !Rm) | (Sm & !Dm & Rm);
    c   = (Sm & !Dm) | (Rm & !Dm) | (Sm & Rm);
    x   = c;

    /* Set initial flags to opposite of what we expect - just to make sure they are modified. */
    SET_NFLG (&regs.ccrflags, n ^ 1);
    SET_ZFLG (&regs.ccrflags, z ^ 1);
    SET_VFLG (&regs.ccrflags, v ^ 1);
    SET_CFLG (&regs.ccrflags, c ^ 1);
    SET_XFLG (&regs.ccrflags, x ^ 1);

    if (size == 8) {
	optflag_subb (&regs, res2, (uae_s8) src, (uae_s8) dst);
	res2 &= 0xFF;
    } else if (size == 16) {
	optflag_subw (&regs, res2, (uae_s16) src, (uae_s16) dst);
	res2 &= 0xFFFF;
    } else
	optflag_subl (&regs, res2, (uae_s32) src, (uae_s32) dst);

    /* Check the result */
    if (res2 != res ||  GET_XFLG(&regs.ccrflags) != x || GET_NFLG(&regs.ccrflags) != n || GET_ZFLG(&regs.ccrflags) != z ||
			GET_VFLG(&regs.ccrflags) != v || GET_CFLG(&regs.ccrflags) != c) {
	printf ("SUB.%c(%08x,%08x) failed. ", size == 8 ? 'B' : size == 16 ? 'W' : 'L', dst, src);
	printf ("Expected=%08x; got=%08x. Expected flags=", res, res2);
	print_flags (x, n, z, v, c);
	printf ("; got flags=");
	print_flags (GET_XFLG(&regs.ccrflags), GET_NFLG(&regs.ccrflags), GET_ZFLG(&regs.ccrflags),
		     GET_VFLG(&regs.ccrflags), GET_CFLG(&regs.ccrflags));
	printf (".\n");

	num_fails++;
    }
    return res2;
}

static uae_u32 test_cmp (uae_u32 src, uae_u32 dst, int size, uae_u32 x)
{
    uae_u32 res;
    uae_u32 Sm, Dm, Rm;
    uae_u32 n, z, v, c;

    num_tests++;

    /* Expected result - just used here for figuring out the expected flags. */
    res = dst - src;
    if (size == 16)
	res &= 0xFFFF;
    else if (size==8)
	res &= 0xFF;

    Sm  = (src >> (size - 1)) & 1;
    Dm  = (dst >> (size - 1)) & 1;
    Rm  = (res >> (size - 1)) & 1;

    /* Expected flags. See, for example, M68000 Programmer's Reference Manual, Appendix A. */
    n   = Rm;
    z   = (res == 0) ? 1 : 0;
    v   = (!Sm & Dm & !Rm) | (Sm & !Dm & Rm);
    c   = (Sm & !Dm) | (Rm & !Dm) | (Sm & Rm);

    /* Set initial flags to opposite of what we expect - just to make sure they are modified. */
    SET_NFLG (&regs.ccrflags, n ^ 1);
    SET_ZFLG (&regs.ccrflags, z ^ 1);
    SET_VFLG (&regs.ccrflags, v ^ 1);
    SET_CFLG (&regs.ccrflags, c ^ 1);
    SET_XFLG (&regs.ccrflags, x);     /* X should be unchanged. */

    if (size == 8)
	optflag_cmpb (&regs, (uae_s8) src, (uae_s8) dst);
    else if (size == 16)
	optflag_cmpw (&regs, (uae_s16) src, (uae_s16) dst);
    else
	optflag_cmpl (&regs, (uae_s32) src, (uae_s32) dst);

    /* Check the result */
    if (GET_XFLG(&regs.ccrflags) != x || GET_NFLG(&regs.ccrflags) != n || GET_ZFLG(&regs.ccrflags) != z ||
	GET_VFLG(&regs.ccrflags) != v || GET_CFLG(&regs.ccrflags) != c) {
	printf ("CMP.%c(%08x,%08x) failed. ", size == 8 ? 'B' : size == 16 ? 'W' : 'L', dst, src);
	printf ("Expected flags=");
	print_flags (x, n, z, v, c);
	printf ("; got flags=");
	print_flags (GET_XFLG(&regs.ccrflags), GET_NFLG(&regs.ccrflags), GET_ZFLG(&regs.ccrflags),
		     GET_VFLG(&regs.ccrflags), GET_CFLG(&regs.ccrflags));
	printf (".\n");

	num_fails++;
    }
    return 0;
}

int main (void)
{
    uae_u32 n[] = {
	0, 1, 2, 3, 4, 7, 8, 10, 50, 100, 126, 127, 128, 129, 130, 150, 200,
	254, 255, 256, 257, 1000, 5000, 100000, 20000, 30000, 32767, 32768,
	327689, 40000, 50000, 60000, 65534, 65535, 65536, 65537, 100000,
	1000000, (1 << 30) - 1, (1 << 30), (1 << 30) + 1, -2L, -1L
    };

    int n_count = sizeof n / sizeof (uae_u32);
    int i;
    int j;
    int count = 0;
    volatile uae_u32 result;

    for (i = 0; i < n_count; i++) {

	uae_u32 dstval = n[i];

	if (dstval <= 0xFF) {
	    result = test_test (dstval, 8, 1);
	    result = test_test (dstval, 8, 1);
	    count += 2;
	}
	if (dstval <= 0xFFFF) {
	    result = test_test (dstval, 16, 0);
	    result = test_test (dstval, 16, 1);
	    count += 2;
	}
	{
	    result = test_test (dstval, 32, 0);
	    result = test_test (dstval, 32, 1);
	    count += 2;
	}


	for (j = 0; j < n_count; j++) {

	    uae_u32 srcval = n[j];


	    if (dstval <= 0xFF && srcval <= 0xFF) {

		result = test_add (dstval, srcval, 8);
		result = test_sub (dstval, srcval, 8);
		result = test_cmp (dstval, srcval, 8, 0);
		result = test_cmp (dstval, srcval, 8, 1);
		count += 4;
	    }
	    if (dstval <= 0xFFFF && srcval <= 0xFFFF) {

		result = test_add (dstval, srcval, 16);
		result = test_sub (dstval, srcval, 16);
		result = test_cmp (dstval, srcval, 16, 0);
		result = test_cmp (dstval, srcval, 16, 1);
		count += 4;
	    }
	    {
		result = test_add (dstval, srcval, 32);
		result = test_sub (dstval, srcval, 32);
		result = test_cmp (dstval, srcval, 32, 0);
		result = test_cmp (dstval, srcval, 32, 1);
		count += 4;
	    }
	}
    }

    if (count != num_tests)
	printf ("Failed: test counts don't match.\n");

    if (num_fails)
	printf ("%d tests failed.\n", num_fails);
   return 0;
}
