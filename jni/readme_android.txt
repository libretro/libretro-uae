		
		E-UAE-0.8.29-WIP4 LIBRETRO DEBUG VERSION


	the Android version is experimental and still buggy .



Compile:

	Go to jni directory, and type ndk-build .

 
Setup:

	- First , create a folder on /mnt/sdcard/euae/ .
	- Put an uae.cfg in this folder
	- Put a ROM and some adf/dms/hdv files in this folder.
	- optional for savestates , create a folder on /mnt/sdcard/euae/save/

Usage:

	Launch retroarch , select a startup dsk , and e-uae sould boot.

	
Knows Bugs: 
 
	- the startup disk choose at start of retroarch is not mounted , use uae.cfg or gui to mount a disk
	- Everything not working well, It's a debug release , so expect to more bug.
