/*
 * Transrom V4.1
 * Copyright 1995,1996 Bernd Schmidt, Marcus Sundberg, Stefan Ropke,
 *                     Rodney Hester
 *
 * Use DICE and 2.0 includes or above to compile
 */

#include <stdio.h>

int main(int argc, char **argv)
{
    int l;

    if (l=OpenLibrary("exec.library",35L)) /* Not sure about V35 */
    {        
	fprintf(stderr,"Writing 512K Kickstart image...\n");
	fflush(stderr);
	fwrite((char *)0xF80000,sizeof(char),0x80000,stdout);
	CloseLibrary(l);
    }
    else
    {
	fprintf(stderr,"Writing 256K Kickstart image...\n");
	fflush(stderr);
	fwrite((char *)0xF80000,sizeof(char),0x40000,stdout);
    }
    return 0;
}
