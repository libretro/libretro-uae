E-UAE 0.8.29-WIP4 for PS3  release 8
====================================
- compiled by using psl1ght v.2 sdk 
- The full uae documentation is stored in the 'docs' subdirectory, here
  you are reading the ps3 port info.

Features:
- 68000, 68010, 68020, 68040, 68060 processor cores
- OCS, ECS and AGA chipset 
- amiga hi-res (720x576) graphics resolution in 32bit depth
- joystick1, joystick2 (2nd joypad required) and mouse emulation
- options dialog (supports switching of the disk files and more)
- sound in 48000 kHz
- keyboard emulation
- support for usb keyboard and usb mouse
- support for loading zip and dms floppy images
- support hor harddisk files
- save-state saving and loading 
- reads either /dev_usb000/uae/uae.cfg or /dev_hdd0/game/EUAE00829/USRDIR/UAE.CFG
  configuration during startup!

Requires:
- ps3 ready to boot homebrew programs
- kickstart rom - see docs/readme

Controls:
- CROSS    - joystick button 1
- SQUARE   - mouse button 1
- TRIANGLE - mouse button 2
- CIRCLE   - user defined keyboard key
- press R2 to activate virtual keyboard (vkb), release R2 to hide it.
  If vkb is shown press left/right digital pad to select the current
  key. Press the 'up' digital pad button to actually simualte key press
  of the current key. L1, L2, R1 ann CIRCLE buttons are user defined keys. 
  If vkb is shown press L1 to define L1 button, press L2 to define L2 button
  and press R1 to define R1 button. Later on (when keyboard is hidden) 
  if you press L1, L2 etc. it will simulate assigned key press / release.
- joypad 1 (emulates joystick 1 and mouse 1)
- joypad 2 (emulates joystick 2) - if you want to enable 2-joystick mode, plug-in
  the second joy pad, restart uae and switch the port mode to "joy-1 & joy-2" in the
  options dialog.
- left analog pad emulates either mouse movement (by default) or joystick. 
  Press R3 (right hat) to switch between mouse and joystick control on left hat.
  Speed-up of the mouse cursor movement can be changed in the options dialog.
- press Start to enter application menu, you can load ADF files from there.
- press CIRCLE to exit the application menu 

Additional info:
- The program reads uae.cfg during startup. In this configuration file
  you should specify all required parameters (memory size, kick rom, initial 
  floppy disk files, cpu core, chipset etc.). 
- Files (kickrom, adf files) are accesible from either usb disk or internal disk.
- NTSC support: press and hold CIRCLE button during emulator startup to initialize
  480 lines /60 Hz video mode. If it doesn't work for you, you can press and hold
  TRIANGLE button during starup to use the current video mode set in dashboard.
  Make sure to change emulator options to actually emulate ntsc amiga machine by 
  setting ntsc=true in your configuration file or in options.
- Save states are stored in your harddisk directory 
  /dev_hdd0/game/EUAE00829/USRDIR/SAVE
- to use harddisk file: enter options and mount it the "Hard files" submenu.
  Alternatively you can specify the harddisk in your configuration file 
  by adding the  following line:
hardfile=read-write,32,1,2,512,/dev_usb000/uae/system.hdf
  or, if the previous one doesn't work for you:
hardfile=read-write,0,0,0,512,/dev_usb000/uae/system.hdf
  Normally you just need to change the path and filename of the hardfile. The 
  first four numbers are the default ones and should work even if your emulated 
  disk file has a different size. The first set of numbers is ok for hardfiles
  formated to OFS (old file system), the second set (zeros) is for harddisk files
  formated to RDB (fast file system). 


Credits and thanks:
- UAE team and porters.
- RCDRUMMOND for E-UAE
- psl1ght sdk team
- PS3 homebrew scene


Release log:
============
r8
- added support for hardware keyboard and mouse plugged-in the ps3 usb ports
- added scan-line support with user defined intensity (options -> video -> scan lines)
  Note: this feature is processor intensive and slows down emulation about 10 to 20%. 
- fixed bugs related to options ui (when program started with default uae.cfg)
- source code ported to psl1ght v.2

r7
- harddisk file can be mounted/unmounted in options menu
- fix for multiple simultaneous keyboard key presses

r6 
- bugfix release (fixed processor core switch, couple of other glitches)
- added 800% floppy speed option
- added QUIT E-UAE option

r5
- updated to the latest  E-UAE 0.8.29-WIP4 source code. 
- support for harddisk files
- suport for save-states and save state thumbnails
- modified options dialog (more options are now configurable)
- support for drive sound (new feature that was not finished in WIP4 code)
- RETURN key added to the virtual keyboard
- left analog stick can be used to emulate joystick  (R3 switches between mouse and joystick
  mode of the left analog stick)
- fixed 2 controller mode: now 2 mice can be emulated as well.
- options dialog hooked up as an euae's gui interface

r4
- unpublished release, vsync modifications

r3
- options dialog has more space for filenames

r2 
- added support for uae.cfg stored on hdd

r1
- initial release

----------
Ole
