/* UAEGUI.rexx - A nice GUI for uae using MUIREXX
 *
 * How to use it: Open a shell a type "run rx UAEGUI.rexx".
 * Then, run UAE. Once uae's window is opened, the GUI will
 * popup.
 *
 * $VER: UAEGUI.rexx 0.3 (29.01.98) © by Samuel Devulder
 *
 * History: 
 *   v0.1: first try
 *   v0.2: adaptation to MUIRexx3.0 (too bad, scripts for MUIRexx2.0 are
 *         not fully compatible with MUIRexx3.0)
 *   v0.3: added speed gauges
 */

TRUE = 1
FALSE = 0
MUIA_Application_Title = 0x804281b8
MUIA_Application_Version = 0x8042b33f
MUIA_Application_Copyright = 0x8042ef4d
MUIA_Application_Author = 0x80424842
MUIA_Application_Base = 0x8042e07a
MUIA_Application_OpenConfigWindow = 0x804299ba
MUIA_Application_AboutMUI = 0x8042d21d
MUIA_Background    = 0x8042545b
MUII_BACKGROUND    = 128
MUII_SHADOWFILL    = 133
MUIA_Image_FreeHoriz = 0x8042da84
MUIA_Image_FontMatchWidth = 0x804239bf
MUIA_Image_FontMatchHeight = 0x804239c0
MUIA_Width         = 0x8042b59c
MUIA_Image_FontMatch = 0x8042815d
MUIA_Image_FontMatchHeight = 0x80429f26
MUIA_FixWidthTxt   = 0x8042d044
MUIA_Weight        = 0x80421d1f
MUIA_Pressed       = 0x80423535
MUIA_ShowMe        = 0x80429ba8
MUIA_Slider_Horiz  = 0x8042fad1
MUIA_Slider_Min    = 0x8042e404
MUIA_Slider_Max    = 0x8042d78a
MUIA_Slider_Level  = 0x8042ae3a
MUIA_Frame         = 0x8042ac64
MUIA_Gauge_Current = 0x8042f0dd
MUIA_Gauge_Horiz   = 0x804232dd
MUIA_Gauge_Max     = 0x8042bcdb
MUIA_Gauge_Divide  = 0x8042d8df
MUIV_Frame_Text    = 3
MUIV_Frame_Group   = 9

Cycle_Active      = 0x80421788
Menuitem_Shortcut = 0x80422030
Menuitem_Title    = 0x804218be
Selected          = 0x8042654b
Disabled          = 0x80423661

options results

ADDRESS COMMAND

BLK = "\033I[2:00000000:00000000:00000000]"
RED = "\033I[2:ffffffff:00000000:00000000]"
GRN = "\033I[2:00000000:ffffffff:6f000000]"

BLK = BLK||BLK||BLK||BLK
RED = RED||RED||RED||RED
GRN = GRN||GRN||GRN||GRN

/* Setup STDERR */
DO_STDERR = 0;
IF OPEN(STDERR,"CON:////UAEGUI.rexx -- Error messages window/WAIT/AUTO","w") then DO_STDERR = 1

/* Run MUIREXX */
address command 'resident c:wait'
if ~show('p','UAEGUI') then do
   'run >nil: muirexx port UAEGUI'
   cpt=0
   do while ~show('p','UAEGUI')
      'wait 1'
      cpt=cpt+1
      if cpt=60 then do
        if DO_STDERR then do
	  call writeln(STDERR,"Timeout while waiting MUIRexx to start.")
        end
        call quit
      END
   end
end

/* wait for UAE to setup */
if ~show('p','UAE') then do
/*   address command "run <>con: uae" */
   cpt=0
   do while ~show('p','UAE')
      'wait 2'
      cpt=cpt+1
      if cpt=60 then do
        if DO_STDERR then do
	  call writeln(STDERR,"Timeout while waiting UAE to start.")
	end
        address UAEGUI quit
        call quit
      end
   end 
end

address UAEGUI

window ID MAIN COMMAND """QUIT""" PORT UAEGUI TITLE """UAE Gui"""

 menu LABEL "Project"
  item COMMAND '"method 'MUIA_Application_AboutMUI' 0"',
   PORT UAEGUI LABEL "About MUI"
  menu LABEL "Settings"
   item COMMAND '"method 'MUIA_Application_OpenConfigWindow'"',
    PORT UAEGUI LABEL "MUI..."
  endmenu
  item ATTRS Menuitem_Title '-1'
  item COMMAND '"quit"' PORT UAEGUI ATTRS Menuitem_Shortcut 'Q' LABEL "Quit"
 endmenu

 address UAE VERSION NUM;    NUM     = RESULT
 address UAE VERSION AUTHOR; AUTHOR  = RESULT
 address UAE VERSION PORT;   PORTAUT = RESULT

 text ATTRS MUIA_Frame MUIV_Frame_Group MUIA_Background MUII_SHADOWFILL LABEL,
   "\0338\033c\033bUAE v"||NUM%10000||"."||(NUM%100)//100||"."||NUM//100||,
   "\033n - Un*x Amiga Emulator\n"||AUTHOR||"\n\n"||PORTAUT 

 group HORIZ
  group FRAME HORIZ LABEL "Actions"
   button PRESS HELP """This button makes UAE and the GUI exit""",
    COMMAND """QUIT""" PORT UAE LABEL "Quit"
   button PRESS HELP """This button makes UAE do a hard reset""",
    COMMAND """RESET""" PORT UAE LABEL "Reset"
   button PRESS HELP """This button makes UAE enter in debug mode""",
    COMMAND """Debug""" PORT UAE LABEL "Debug"
  endgroup
  group FRAME HORIZ LABEL "Speed"
   LABEL "fps:"
   text ID FPS  ATTRS 0x8042a3f1 4 LABEL "0.00"
   gauge ID SPD,
    HELP """You can read here the approximated speed of\n"||,
           "UAE relative to a real Amiga (50fps).""",
    ATTRS MUIA_Gauge_Horiz TRUE MUIA_Gauge_Max 100 LABEL "%ld%%"
  endgroup
 endgroup

 group  FRAME LABEL "Parameters"
  group HORIZ
   label "Display:"
   address UAE QUERY DISPLAY
   cycle ID DRAW,
    HELP """This cycle button enables or disables the display output""",
    COMMAND """DISPLAY %s""" PORT UAE ATTRS Cycle_Active RESULT labels "OFF,ON"
   label "Sound:"
   address UAE QUERY SOUND
   IF RESULT = -1 THEN
        cycle id sound ATTRS Disabled TRUE COMMAND """SOUND %s""",
         PORT UAE LABELS "OFF,ON,BEST"
   ELSE IF RESULT = 0 THEN
        cycle id sound ATTRS Cycle_Active 0,
         HELP """This cycle button enables or disables the sound output""",
         COMMAND """SOUND %s""" PORT UAE LABELS "OFF,ON,BEST"
   ELSE cycle id sound,
         HELP """This cycle button enables or disables the sound output""",
         ATTRS Cycle_Active RESULT-1 COMMAND """SOUND %s""",
         PORT UAE LABELS "OFF,ON,BEST"	 
/*   label "Joystick:"
   address UAE QUERY FAKEJOYSTICK
   cycle ID JOY,
    HELP """This cycle button enables or disables the joystick emulation""",
    COMMAND """FAKEJOYSTICK %s""" PORT UAE ATTRS Cycle_Active RESULT,
    LABELS "OFF,ON" */
  endgroup
  group HORIZ
   label "POW:"
   address UAE QUERY LED_POW;if RESULT = 1 then COL = RED; else COL = BLK
   button ID POW HELP """This image represents the power-led state""",
    ATTRS MUIA_FixWidthTxt 1 label COL
   label "Frame Rate:"
   address UAE QUERY FRAMERATE
   slider ID SLDR HELP """Use this slider gadget to select the frame rate""",
    COMMAND """FRAMERATE %s""" PORT UAE ATTRS MUIA_Slider_Horiz TRUE,
    MUIA_Slider_Min 1 MUIA_Slider_Max 20 MUIA_Weight 230,
    MUIA_SLIDER_LEVEL RESULT
  endgroup
 endgroup

 group FRAME LABEL "Disk files"
  call SetDfx(0)
  call SetDfx(1)
  call SetDfx(2)
  call SetDfx(3)
 endgroup
endwindow

address UAE QUERY NAME_DF0;R0=RESULT
address UAE QUERY NAME_DF1;R1=RESULT
address UAE QUERY NAME_DF2;R2=RESULT
address UAE QUERY NAME_DF3;R3=RESULT
popasl ID NDF0 CONTENT R0
popasl ID NDF1 CONTENT R1
popasl ID NDF2 CONTENT R2
popasl ID NDF3 CONTENT R3

address UAE feedback LED_POW PORT UAEGUI,
 CMD_ON  """button ID POW LABEL "RED"""",
 CMD_OFF """button ID POW LABEL "BLK""""

address UAE feedback LED_DF0 PORT UAEGUI,
 CMD_ON  """button ID DF0 LABEL "GRN"""",
 CMD_OFF """button ID DF0 LABEL "BLK""""

address UAE feedback LED_DF1 PORT UAEGUI,
 CMD_ON  """button ID DF1 LABEL "GRN"""",
 CMD_OFF """button ID DF1 LABEL "BLK""""

address UAE feedback LED_DF2 PORT UAEGUI,
 CMD_ON  """button ID DF2 LABEL "GRN"""",
 CMD_OFF """button ID DF2 LABEL "BLK""""

address UAE feedback LED_DF3 PORT UAEGUI,
 CMD_ON  """button ID DF3 LABEL "GRN"""",
 CMD_OFF """button ID DF3 LABEL "BLK""""

address UAE feedback NAME_DF0 PORT UAEGUI,
 CMD """popasl ID NDF0 CONTENT %s"""

address UAE feedback NAME_DF1 PORT UAEGUI,
 CMD """popasl ID NDF1 CONTENT %s"""

address UAE feedback NAME_DF2 PORT UAEGUI,
 CMD """popasl ID NDF2 CONTENT %s"""

address UAE feedback NAME_DF3 PORT UAEGUI,
 CMD """popasl ID NDF3 CONTENT %s"""

address UAE feedback ON_EXIT PORT UAEGUI,
 CMD """quit"""

address UAE QUERY LED_POW;if RESULT = 1 then COL = RED; else COL = BLK
button ID POW label COL

address UAE QUERY FRAMENUM;FN = RESULT; TN = time('r');

/* in case UAEGUI exits */
signal on syntax
signal on error
ok = 1
gauge ID SPD ATTRS MUIA_Gauge_Horiz TRUE MUIA_Gauge_Max 100
do while ok
  address command wait 5
  address UAE QUERY FRAMENUM;FN0=FN;FN=RESULT;TN = time('r');
  if TN~=0 then TN = (FN-FN0)/TN
  if show('p','UAEGUI') then do
    text  id FPS label trunc(100*TN)/100
    gauge ID SPD ATTRS MUIA_Gauge_Current trunc(TN*2)
  end; else ok = 0
end

error:
syntax:

quit:

if show('p','UAE') then do
  address UAE feedback LED_POW PORT COMMAND CMD_ON  """"""
  address UAE feedback LED_DF0 PORT COMMAND CMD_ON  """"""
  address UAE feedback LED_DF1 PORT COMMAND CMD_ON  """"""
  address UAE feedback LED_DF2 PORT COMMAND CMD_ON  """"""
  address UAE feedback LED_DF3 PORT COMMAND CMD_ON  """"""
  address UAE feedback LED_POW PORT COMMAND CMD_OFF """"""
  address UAE feedback LED_DF0 PORT COMMAND CMD_OFF """"""
  address UAE feedback LED_DF1 PORT COMMAND CMD_OFF """"""
  address UAE feedback LED_DF2 PORT COMMAND CMD_OFF """"""
  address UAE feedback LED_DF3 PORT COMMAND CMD_OFF """"""
end

if DO_STDERR then call close(STDERR);

exit 0

SetDFx: 
   PARSE ARG unit
   group ID GDF||unit HORIZ
    label LEFT DOUBLE "DF"||unit||":"
    address UAE QUERY LED_DF||unit;if RESULT=1 then COL=GRN; else COL=BLK
    button ID DF||unit,
     HELP """This image represents the state of drive "||unit||"'s led""",
     ATTRS MUIA_FixWidthTxt 1 label COL
    button PRESS,
     HELP """Use this button to eject the diskfile in drive DF"||unit||":""",
     COMMAND """EJECT "||unit||"""" PORT UAE ATTRS MUIA_Weight 100,
     LABEL "Eject"
    popasl ID NDF||unit,
     HELP """Select the name of diskfile for drive DF"||unit||":""",
     COMMAND """INSERT "||unit||" '%s'""" PORT UAE ATTRS MUIA_Weight 300
   endgroup ID GDF||unit
return
