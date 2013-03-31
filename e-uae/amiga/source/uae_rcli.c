;/* compile with: execute uae_rcli.c
failat 11
sc uae_rcli.c opt link to uae_rcli noicons
if ERROR
   gcc uae_rcli.c -O3 -o uae_rcli -noixemul
endif
quit
*/

/*
 *  uae_rcli.c - UAE remote cli
 *
 *  (c) 1997 by Samuel Devulder
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <libraries/dosextens.h>

#ifdef __GNUC__
#include "../../src/include/uaeexe.h"
#else
#include "//src/include/uaeexe.h"
#endif

#define NAME "uae_rcli"
#define LEN 512

static int (*calltrap)(char *, int) = (void*)UAEEXE_ORG;
static int delay  = 20; 
static int debug  = 0;
static int nofifo = 0;
static ULONG fifo;
static char buf[LEN];

/*
 * lowlevel I/O
 */
static void WR(ULONG f,char *s)
{
    Write(f,s,strlen(s));
}
static void PR(char *s)
{
    WR(Output(),s);
}

/*
 * self explanatory
 */
static void usage(char *name)
{
    PR("Usage: ");PR(name);PR(" [-h|?] [-debug] [-nofifo] [<delay>]\n");
    exit(0);
}

/*
 * grab options
 */
static void parse_cmdline(int ac, char **av)
{
    char *name = *av++;

    for(;--ac;++av) {
        if(!strcmp(*av,"-debug"))  debug  = 1;        else
        if(!strcmp(*av,"-nofifo")) nofifo = 1;        else
        if(!strcmp(*av,"-h")) usage(name);            else
        if(!strcmp(*av,"?"))  usage(name);            else
        if(**av>='0' && **av<='9') delay = atoi(*av); else
        {PR("Bad argument: \"");PR(*av);PR("\"\n");exit(0);}
    }
    if(!delay) delay = 1;
}

/*
 * See if command matches. Returns pointer to arguments.
 */
static char *match(char *src,char *cmd)
{
    while(*src == ' ' || *src == '\t') ++src;
    while(*src && tolower(*src) == tolower(*cmd)) {++src;++cmd;}
    while(*src==' ' || *src=='\t') ++src;
    return (*cmd)?NULL:src;
}

/*
 * get command
 */
static int getcmd(void)
{
    if(debug) PR("-> Calltrap\n");
    if(calltrap(buf, LEN-1)) {
        if(debug) PR("-> 1\n");
        return 1;
    } else {
        if(debug) PR("-> 0\n");
        return 0;
    }
/*
    PR(">> ");
    if(fgets(buf,LEN-1,stdin) == NULL) strcpy(buf,"quit");
    if(*buf=='\n') return 0;
    return 1;
*/
}

/*
 * execute command
 */
static void my_exec(void)
{
    if(debug) {PR("-> Exec \"");PR(buf);PR("\"\n");}
    if(fifo) {
        WR(fifo,buf);
        WR(fifo,"\n");
    } else { /* nofifo => emulate cli */
        char *s;
        if((s=match(buf,"cd"))) {
            if(*s==';' || !*s) {
                char buff[128];
                getcwd(buff,sizeof(buff)-1);
                PR(buff);PR("\n");
            } else {
                chdir(s);
            }
        } else {
            System(buf, NULL);
        }
    }
    *buf = '\0';
}

/*
 * Open without requester
 */
ULONG myOpen(char *name, ULONG mode)
{
    ULONG rt, wd;
    struct Process *pr;

    pr = (void*)FindTask(NULL);
    if(pr->pr_Task.tc_Node.ln_Type != NT_PROCESS) return 0;

    wd = (ULONG)pr->pr_WindowPtr;
    pr->pr_WindowPtr = (APTR)-1;
    rt = Open(name,mode);
    pr->pr_WindowPtr = (APTR)wd;

    return rt;
}

/*
 * Guess :)
 */
int main(int ac, char **av)
{
    int running = 1;

    parse_cmdline(ac,av);
    if(!nofifo) {
        if(debug) PR("-> Open fifo\n");
        fifo = myOpen("fifo:"NAME"/wmke",MODE_NEWFILE);
        if(!fifo) {
            if(debug) PR("-> Starting fifo-handler\n");
            System("run <nil: >nil: l:fifo-handler",NULL);
            Delay(100);
            if(debug) PR("-> Reopen fifo\n");
            fifo = myOpen("fifo:"NAME"/wmke",MODE_NEWFILE);
        }
    }

    if(fifo) {
        if(debug) PR("-> Spawning shell\n");
        System("run execute fifo:"NAME"/rsk",NULL);
        if(debug) WR(fifo,"echo \"-> Remote cli running\"\n");
    } else if(debug) PR("-> No fifo found\n");

    do {
        while(running && getcmd()) {
            if(match(buf,"endcli"))   running = 0; else
            if(match(buf,"endshell")) running = 0; else
            if(match(buf,"quit"))     running = 0;
	    if(SetSignal(0L, SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D) & 
	                    (SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D)) {
		running = 0;
	    }
            if(running) my_exec();
        }
        if(running) Delay(delay);
    } while(running);
    if(debug) PR("-> Exiting\n");
    (*calltrap)(0,0);
    if(fifo) {
        Close(fifo);
    }
}
