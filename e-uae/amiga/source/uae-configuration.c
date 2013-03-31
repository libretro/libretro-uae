
#define __NOLIBBASE__
#include <proto/exec.h>
#include <proto/dos.h>

#define OUTBUFSIZE 1000

long (*UaeConf) (...);

struct Library *SysBase;
struct Library *DOSBase;

char outbuf[OUTBUFSIZE+1];

__saveds long start (register __a0 param,register __d0 paramlen)
{
	long rc = 20;
	long index;
	BPTR out;
	char *s;
	BOOL brk = FALSE;

	UaeConf = (void *) 0xf0ff60;

	SysBase = *((struct Library **)4);

	if (DOSBase = OpenLibrary ("dos.library",0))
	{
		out = Output();

		if (*((ULONG *)UaeConf))
		{
			index = -1;

			do	{
				outbuf[0] = 0;
				rc = UaeConf (82,index,param,paramlen,outbuf,OUTBUFSIZE);

				if (outbuf[0] && out)
				{
					for (s = outbuf; *s; s++);
					*s++ = '\n';
					Write (out,outbuf,s - outbuf);
				}

				index++;
				if (SetSignal(0,0) & SIGBREAKF_CTRL_C)
					brk = TRUE;
			} while (!brk && rc < 0);

			if (brk && out)
			{
				Write (out,"*** Break\n",10);
				rc = 10;
			}

		}
		else
			if (out)
				Write (out,"Your version of WinUAE is too old\n",34);

		CloseLibrary (DOSBase);
	}

	return (rc);
}
