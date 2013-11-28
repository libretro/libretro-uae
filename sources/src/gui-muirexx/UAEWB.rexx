/*rx
 * UAE.wb - A nice GUI for launching uae using MUIREXX
 *
 * (c) By Samuel Devulder, 01/98.
 * (c)    Richard Drummond 12/2004
 *
 * $VER: UAEWB.rexx (01.12.2004) © by Samuel Devulder
 *
 * History:
 *   v0.1 (14.01.98): first try
 *   v0.2 (26.01.98): adaptation to MUIRexx3.0 (too bad, scripts for
 *                    MUIRexx2.0 are not fully compatible with MUIRexx3.0)
 *   v0.3 (28.02.98): added "quality" parameter for sound.
 *   v0.4 (01.05.98): added support for CPU selection.
 *   v0.5 (01.12.04): added support for selecting ROM key file
 */

OPTIONS RESULTS
ADDRESS COMMAND

/* Run MUIREXX */
IF ~show('p','UAEWB') THEN DO
   'run >nil: muirexx port UAEWB'
   DO WHILE ~show('p','UAEWB')
      'wait 1'
   END
END

ADDRESS UAEWB

WINDOW ID WAIT ATTRS 0x80421923 0 0x8042045d 0
  LABEL "\033b\033cPlease wait\nwhile UAE is loading...\033n"
ENDWINDOW

TRUE = 1
FALSE = 0
MUIA_Application_Title     = 0x804281b8
MUIA_Application_Version   = 0x8042b33f
MUIA_Application_Copyright = 0x8042ef4d
MUIA_Application_Author    = 0x80424842
MUIA_Application_Base      = 0x8042e07a
MUIA_Application_OpenConfigWindow = 0x804299ba
MUIA_Application_AboutMUI  = 0x8042d21d
MUIA_Background            = 0x8042545b
MUIA_Image_FreeHoriz       = 0x8042da84
MUIA_Image_FontMatchWidth  = 0x804239bf
MUIA_Image_FontMatchHeight = 0x804239c0
MUIA_Width                 = 0x8042b59c
MUIA_Image_FontMatch       = 0x8042815d
MUIA_Image_FontMatchHeight = 0x80429f26
MUIA_FixWidthTxt           = 0x8042d044
MUIA_Weight                = 0x80421d1f
MUIA_Pressed               = 0x80423535
MUIA_ShowMe                = 0x80429ba8
MUIA_Slider_Horiz          = 0x8042fad1
MUIA_Slider_Min            = 0x8042e404
MUIA_Slider_Max            = 0x8042d78a
MUIA_Slider_Level          = 0x8042ae3a
MUIA_Frame                 = 0x8042ac64
MUIA_Screen_Accept         = 0x8042e3e1
MUIA_Screen_MaxLen         = 0x80424984
MUIA_List_Format           = 0x80423c0a
MUIA_Cycle_Active          = 0x80421788
MUIA_Menuitem_Shortcut     = 0x80422030
MUIA_Menuitem_Title        = 0x804218be
MUIA_Selected              = 0x8042654b
MUIA_Disabled              = 0x80423661
MUIA_Group_Columns         = 0x8042f416
MUIA_ControlChar           = 0x8042120b
ASLFR_DrawersOnly          = 0x8008002F

MUIV_Frame_Text = 3
MUIV_Frame_Group = 9

MUII_BACKGROUND    = 128
MUII_SHADOWFILL    = 133
MUII_HSHADOWBACK   = 140
MUII_HSHADOWSHADOW = 142
WINDOW ID MAIN COMMAND """QUIT""" PORT UAEWB TITLE """UAE Launcher"""

  MENU LABEL "Project"
    MENU LABEL "About"
         ITEM COMMAND '"UAEWB.aux about"',
             ATTRS MUIA_Menuitem_Shortcut 'A',
             LABEL "UAE"
         ITEM COMMAND '"method 'MUIA_Application_AboutMUI' 0"',
             ATTRS MUIA_Menuitem_Shortcut 'M',
             PORT UAEWB LABEL "MUI"
    ENDMENU
    MENU LABEL "Settings"
         ITEM COMMAND '"UAEWB.aux a500"'    LABEL "Preset to A500"
         ITEM COMMAND '"UAEWB.aux HAM"'     LABEL "Set output to HAM6"
         ITEM ATTRS MUIA_Menuitem_Title '-1'
         ITEM COMMAND '"UAEWB.aux default"' LABEL "Reset to default"
         ITEM COMMAND 'ENV:UAEWB'           LABEL "Reset to last saved"
         ITEM ATTRS MUIA_Menuitem_Title '-1'
         ITEM COMMAND '"method 'MUIA_Application_OpenConfigWindow'"',
              PORT UAEWB LABEL "MUI..."
    ENDMENU
    ITEM ATTRS MUIA_Menuitem_Title '-1'
    ITEM COMMAND '"quit"' PORT UAEWB ATTRS MUIA_Menuitem_Shortcut 'Q',
         LABEL "Quit"
  ENDMENU

  /* Presentation txt */
  TEXT ATTRS MUIA_Frame MUIV_Frame_Group MUIA_Background MUII_HSHADOWSHADOW,
       LABEL "\0338\033c\033bUAE Launcher v0.4\033n",
             "\n© by Samuel Devulder, 05/98."

  SPACE

  /* Main group */
  GROUP REGISTER LABELS "Display,Memory,Disks,Sound,Misc"
    /* DISPLAY */
    GROUP FRAME LABEL "Gfx Setup"
      GROUP HORIZ
        LABEL "Size:"
        CYCLE ID SIZE,
              HELP """LORES makes UAE use a 320x256 window "||,
                     "(\033ifast and good for games\033n)\n"||,
                     "HIRES makes UAE use a 640x512 window "||,
                     "(\033igood for Workbench\033n)""",
              LABELS "LORES,HIRES"
        LABEL "Screen:"
        CYCLE ID SCRN,
              HELP """PUBLIC => Workbench output\n"||,
                     "ASLREQ => ASL-Requester selection\n"||,
                     "DEFSCR => Automatically Selected screen\n"||,
                     "ModeID => ModeID and Depth below define screen.""",
              LABELS "PUBLIC,ASLREQ,DEFSCR,ModeID"
      ENDGROUP
      GROUP HORIZ
        LABEL "ModeID:"
        STRING ID SM,
               HELP """Enter the screen mode you want UAE to use.\n"||,
                      "(format 0xNNNNNNNN).""",
               PORT UAEWB COMMAND """CYCLE ID SCRN ATTRS 0x80421788 3"""
        LABEL "Depth:"
        CYCLE ID DEPTH,
              HELP """Select the depth of UAE's screen. (\033iUse\n"||,
                     "this in conjunction with ModeID\033n)""",
              PORT UAEWB COMMAND """CYCLE ID SCRN ATTRS 0x80421788 3""",
              ATTRS MUIA_Weight 1,
              LABELS "N/A,1,2,3,4,5,6,7,8,15,16,24,32"
      ENDGROUP
      GROUP HORIZ
        LABEL "Frame Rate:"
        SLIDER ID SLDR,
               HELP """Use this slider gadget to select the frame rate\n"||,
                      "(\033ithe lower, the smoother the display will "||,
                      "be\033n).""",
               MUIA_Slider_Horiz TRUE MUIA_Slider_Min 1 MUIA_Slider_Max 20
      ENDGROUP
      GROUP HORIZ
        SPACE HORIZ
        LABEL "Gray scale:"
        CHECK ID GRAY,
          HELP """Make UAE use a gray output\n(\033iusefull for WB with few"||,
                 " colors\033n)"""
        SPACE HORIZ
        LABEL "Dithering:";
        CHECK ID DITH ATTRS MUIA_Selected TRUE,
          HELP """If activated, UAE will use dithering to approximate "||,
                 "colors.\n(\033idisable this if you want to emulate "||,
                 "WB-based programs\nor to gain some speed\033n)"""
        SPACE HORIZ
      ENDGROUP
    ENDGROUP
    /* MEMORY */
    GROUP
      GROUP HORIZ FRAME ATTRS MUIA_Group_Columns 4 LABEL "RAM"
        LABEL "ChipRam:"
        CYCLE ID CHIP,
          ATTRS MUIA_Cycle_Active 2,
          HELP """Select the amount of ChipRam UAE will emulate\n"||,
                 "(\033i2Mb max if FASTRAM is used\033n).""",
          LABELS "512 Kb,1 Mb,2 Mb,4 Mb,8 Mb"
        LABEL "SlowFast:"
        CYCLE ID SLOW,
          HELP """Select the amount of FASTRAM size that UAE\n"||,
                 "will emulate at 0xC00000.""",
          LABELS "None,512 Kb,1024 Kb"
        LABEL "FastRam:"
        CYCLE ID FAST,
          HELP """Select the amount of 32bit FASTRAM that UAE\n"||,
                 "will emulate at 0x200000.""",
          LABELS "None,1 Mb,2 Mb,3 Mb,4 Mb,5 Mb,6 Mb,7 Mb,8 Mb"
        LABEL "ZorroIII: "
        GROUP HORIZ
          POPSLIDER ID Z3RAM,
            ATTRS MUIA_Slider_Level 0 MUIA_Slider_Min 0 MUIA_Slider_Max 64,
            HELP """Select the amount of 32bit Zorro-III\nram UAE will "||,
                   "emulate."""
          LABEL "Mb"
          SPACE HORIZ
        ENDGROUP
      ENDGROUP
      GROUP HORIZ FRAME ATTRS MUIA_Group_Columns 2 LABEL "ROM"
        LABEL "Romfile:"
        POPASL ID ROMF HELP """Enter the romfile UAE will use.""",
               CONTENT ""
	LABEL "Keyfile:"
	POPASL ID KEYF HELP """Enter the keyfile for encrypted ROMs.""",
	       CONTENT ""
      ENDGROUP
    ENDGROUP
    /* DISKS */
    GROUP
      GROUP HORIZ FRAME ATTRS MUIA_Group_Columns 4 LABEL "Diskfiles"
        LABEL "DF0:"
        POPASL ID DF0 HELP """Enter the diskfile for emulating DF0:"""
        LABEL "DF1:"
        POPASL ID DF1 HELP """Enter the diskfile for emulating DF1:"""
        LABEL "DF2:"
        POPASL ID DF2 HELP """Enter the diskfile for emulating DF2:"""
        LABEL "DF3:"
        POPASL ID DF3 HELP """Enter the diskfile for emulating DF3:"""
      ENDGROUP
      GROUP HORIZ ATTRS MUIA_Group_Columns 4
        LABEL "HDisk 1:"
        STRING ID VMOU1 HELP """Enter the name of the 1st hard-disk (or partition)\n"||,
                               "UAE will mount (\033iUse Misc/Extra param to mount "||,
                               "more\nthan 3 hard-disks\033n)."""
        LABEL '"="'
        POPASL ID VPAT1 ATTRS ASLFR_DrawersOnly 1 MUIA_Weight 300,
            HELP """Enter the path to use for the 1st hard-disk (or partition)"""
        LABEL "HDisk 2:"
        STRING ID VMOU2 HELP """Enter the name of the 2nd hard-disk (or partition)\n"||,
                               "UAE will mount (\033iUse Misc/Extra param to mount "||,
                               "more\nthan 3 hard-disks\033n)."""
        LABEL '"="'
        POPASL ID VPAT2 ATTRS ASLFR_DrawersOnly 1 MUIA_Weight 300,
            HELP """Enter the path to use for the 2nd hard-disk (or partition)"""
      ENDGROUP
    ENDGROUP
    /* SOUND */
    GROUP HORIZ FRAME LABEL "Audio Setup"
      SPACE HORIZ
      GROUP HORIZ ATTRS MUIA_Group_Columns 2 muia_weight 400
        LABEL "Sound:"
        CYCLE ID SOUND,
              HELP """NONE => No sound\n"||,
                     "OFF  => Emulated but no output\n"||,
                     "ON   => Emulated\n"||,
                     "BEST => Accurate emulation (\033iCPU power "||,
                     "needed\033n).""",
              LABELS "NONE,OFF,ON,BEST"
        LABEL "Quality:"
        GROUP HORIZ
          CYCLE ID AUDBIT,
              HELP """16 bits will only work if AHI is installed""",
              LABELS "8 bits,16 bits"
          CYCLE ID AUDMS,
              HELP """Stereo will only work if AHI is installed""",
              LABELS "mono,stereo"
        ENDGROUP
        LABEL "Frequency:"
        GROUP HORIZ
          POPSLIDER ID FREQ,
            HELP """Select the frequency UAE will use to\n"||,
                   "mix output (the more, the better [\033ibut\n"||,
                   "the more CPU power is needed too\033n]).""",
            ATTRS MUIA_Slider_Level 11 MUIA_Slider_Min 4 MUIA_Slider_Max 44
          LABEL "KHz"
          SPACE HORIZ
        LABEL "Buffer:"
        GROUP HORIZ
          POPSLIDER ID BUFF,
            HELP """Select the sound buffer size for UAE\n"||,
                   "(the more, the better).""",
            ATTRS MUIA_Slider_Level 2 MUIA_Slider_Min 8 MUIA_Slider_Max 256
          LABEL "Kb"
        ENDGROUP
        ENDGROUP
        LABEL "Soundfile:"
        POPASL ID SFILE,
          HELP """\033bEXPERT OPTION\033n: Select the filename UAE\n"||,
                 "will use to output sound."""
      ENDGROUP
      SPACE HORIZ
    ENDGROUP
    /* MISC */
    GROUP
      GROUP HORIZ ATTRS MUIA_Group_Columns 4
        LABEL "FastBlt:"
        CYCLE ID FBLT,
          HELP """When activated, UAE will emulate\na 32bit fast blitter.""",
          LABELS "ON,OFF"
        LABEL "GUI:"
        CYCLE ID GUI,
          HELP """If not enabled, UAE will run without GUI.""",
          LABELS "Enabled,Disabled"
        LABEL "Port 0:"
        CYCLE ID JOY0,
          ATTRS MUIA_Disabled FALSE,
          LABELS "Mouse,Joystick,KeyPad,Arrows"
        LABEL "Port 1:"
        CYCLE ID JOY1,
          ATTRS MUIA_Cycle_Active 1 MUIA_Disabled FALSE,
          LABELS "Mouse,Joystick,KeyPad,Arrows"
        LABEL "CPU:"
        CYCLE ID CPUT,
          HELP """Select the CPU you want to emulate:\n"||,
                 "  - 680x0   => No need explain :-)\n"||,
                 "  - 020/881 => 68020 (or 68EC030) with fpu\n"||,
                 "  - mc68k   => more compatible (but slower) 68000 mode""",
          ATTRS MUIA_Cycle_Active 2,
          LABELS "68000,68010,68020,020/881,mc68k"
        LABEL "Power:"
        SLIDER ID CPUR,
            HELP """\033bEXPERT OPTION\033n:\n"||,
                   "1 => More time spent in CPU emulation "||,
                                   "(good for Workbench)\n"||,
                   "4 => Normal CPU vs CHIPSET time ratio\n"||,
                   "8 => More time spent in CHIPSET emulation "||,
                                   "(good for games/demos)""",
            ATTRS MUIA_Slider_Horiz TRUE MUIA_Slider_Level 4,
                  MUIA_Slider_Min 1 MUIA_Slider_Max 16
      ENDGROUP
      GROUP HORIZ ATTRS MUIA_Group_Columns 2
        LABEL "AutoGrap IFF:"
        POPASL ID SIFF HELP """\033bEXPERT OPTION\033n: Enter here the "||,
                              "filename\nUAE will use to save frames."""
        LABEL "Extra options:"
        STRING ID EXTRA HELP """Write here the extra parameters (see README)\n"||,
                               "you want to pass to UAE."""
      ENDGROUP
    ENDGROUP
  ENDGROUP

  SPACE

  /* SAVE | USE | CANCEL */
  GROUP HORIZ
    button PRESS HELP """Save current setup and launch UAE""",
      COMMAND """UAEWB.aux save""" LABEL "Save"
    button PRESS HELP """Launch UAE with current settings""",
      COMMAND """UAEWB.aux use""" LABEL "Use"
    button PRESS HELP """Exit without launching UAE""",
      COMMAND "quit" PORT UAEWB LABEL "Cancel"
  ENDGROUP
EndWindow

WINDOW ID WAIT CLOSE

ENV="ENV:UAEWB"
IF EXISTS(ENV) THEN ADDRESS COMMAND "rx " ENV

exit 0
