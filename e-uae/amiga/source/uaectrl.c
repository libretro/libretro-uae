/***********************************************************
 * UAE - The U*nix Amiga Emulator			   *
 *							   *
 * UAE-Ctrl -- Emulator Control from Inside Emulation	   *
 *  (c) 1996 Tauno Taipaleenmaki <tataipal@raita.oulu.fi>  *
 *							   *
 * Version 0.1						   *
 *							   *
 * Command line version, Should work with any KS version   *
 *							   *
 ***********************************************************/
#include <clib/exec_protos.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "uae-control.h"
#include "uae_pragmas.h"

#define MAX_DRV_NAME	      20

struct UAE_CONFIG      config;

void print_drive_status(void);
void quit_program(int error, char *text);

/************************************
 * Main program 		    *
 ************************************/
int main()
{
       int		    quit = 0,i, correct,number;
       char		    buf[257];
       char		    *langs[]={
	      "US\0","DE\0","SE\0","FR\0","IT\0",
       };

/* Read UAE configuration */
       i = GetUaeConfig( &config );

       while( quit == 0 ) {
	      printf(" UAE-Control v0.1\n\n");
	      printf(" 1) Reset\n");
	      printf(" 2) Debug\n");
	      printf(" 3) Exit Emulator\n");
	      printf(" 4) Change framerate     (Currently : %ld)\n", config.framerate);
	      printf(" 5) Toggle sound         (Currently : %s)\n", config.do_output_sound ? "ON" : "OFF");
	      printf(" 6) Toggle fake joystick (Currently : %s)\n", config.do_fake_joystick ? "ON" : "OFF");
	      printf(" 7) Change language      (Currently : %s)\n", langs[config.keyboard]);
	      printf(" 8) Eject a disk\n");
	      printf(" 9) Insert a disk\n");
	      printf("10) Exit UAE-Control\n\n");
	      correct = 0;
	      while( correct == 0 ) {
		     printf(" Command : ");
		     gets( buf );
		     i = atoi( buf );
		     if ((i > 0) && (i < 11))
		       correct = 1;
	      }
	      switch( i ) {
	       case 1:
		     HardReset();
		     break;
	       case 2:
		     DebugFunc();
		     break;
	       case 3:
		     ExitEmu();
		     break;
	       case 4:
		     printf(" Enter new framerate (1-20) :");
		     gets( buf );
		     number = atoi( buf );
		     if (SetFrameRate (number))
			    GetUaeConfig(&config);
		     else
			    printf(" Illegal value, not changed.\n");
		     break;
	       case 5:
		     if (config.do_output_sound)
		       DisableSound();
		     else
		       EnableSound();
		     GetUaeConfig( &config );
		     break;
	       case 6:
		     if (config.do_fake_joystick)
		       DisableJoystick();
		     else
		       EnableJoystick();
		     GetUaeConfig( &config );
		     break;
	       case 7:
		     printf(" 1 = US, 2 = DE, 3 = SE, 4 = FR, 5 = IT\n");
		     printf(" What will it be : ");
		     gets( buf );
		     number = atoi( buf );
		     if ((number >= 1) && (number <= 5)) {
			    ChangeLanguage( number-1 );
			    GetUaeConfig( &config );
		     } else {
			    printf(" Illegal value, not changed.\n");
		     }
		     break;
	       case 8:
		     print_drive_status();
		     printf(" Eject which drive (1-4): ");
		     gets( buf );
		     number = atoi( buf );
		     if ((number >= 1) && (number <=4 )) {
			    EjectDisk( number-1 );
			    GetUaeConfig( &config );
		     } else {
			    printf(" Illegal drive, not changed.\n");
		     }
		     break;
	       case 9:
		     print_drive_status();
		     printf(" Enter disk to drive (1-4): ");
		     gets( buf );
		     number = atoi( buf );
		     if ((number >= 1) && (number <= 4)) {
			    printf("Name of diskfile :");
			    gets( buf );
			    InsertDisk( (UBYTE *)&buf, number - 1 );
			    GetUaeConfig( &config );
		     } else {
			    printf(" Illegal drive, not changed.\n");
		     }
		     break;
	       case 10:
		     quit = 1;
		     break;
	      }
       }
       quit_program(0, "");
       return(0);
}

/******************************************
 * Prints drive status			  *
 ******************************************/
void print_drive_status(void)
{
       printf(" DF0 : %s\n", config.disk_in_df0 ? config.df0_name : "EMPTY");
       printf(" DF1 : %s\n", config.disk_in_df1 ? config.df1_name : "EMPTY");
       printf(" DF2 : %s\n", config.disk_in_df2 ? config.df2_name : "EMPTY");
       printf(" DF3 : %s\n", config.disk_in_df3 ? config.df3_name : "EMPTY");
}



/******************************************
 * Quits the program			  *
 ******************************************/
void quit_program(int error, char *text)
{
       if (error > 0) {
	      printf(" UAE-Control v0.1\n");
	      printf("  (c)1996 Tauno Taipaleenmaki\n\n");
	      printf(" ERROR: %s\n", text);
       }
}
