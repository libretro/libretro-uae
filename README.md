# P-UAE LIBRETRO

Based on P-UAE 2.6.1 https://github.com/GnoStiC/PUAE 
Git Commit: 0186c1b16f7181ffa02d73e6920d3180ce457c46

All credits to:

Richard Drummond "http://www.rcdrummond.net/uae/"

E-UAE is based on the work of dozens of contributors including Bernd
Schmidt (the original author and maintainer of UAE), Bernie Meyer (the
author of the x86 JIT compiler), Toni Wilen (the current maintainer of
WinUAE), and many more.

This RETRO port was  based at start on PS3 version E-UAE 0.8.29-WIP4 release 8
(so also credits to Ole.)

For now we use the UAE core provided by P-UAE 2.6.1:
Git Commit: 0186c1b16f7181ffa02d73e6920d3180ce457c46
https://github.com/GnoStiC/PUAE 

Credits to Mustafa 'GnoStiC' TUFAN

And of course for the RetroArch/Libretro team : "http://www.libretro.com/"

## Default Controls

```
L   Toggle virtual keyboard
R   Toggle statusbar
A   Button 2 / RMB
B   Button 1 / LMB
```

## Disk images, WHDLoad and M3U support
You can pass a disk or a hard drive image as a rom.

Supported formats are:
- adf, dms, fdi, ipf, zip files for disk images
- hdf, hdz for hard drive images
- m3u for multiple disk images

When passing these files as parameter the core will generate a temporary uae configuration file in RetroArch saves directory and use it to automatically launch the game.

### Configuration
To generate the temporary uae configuration file the core will use the core options configured in RetroArch.

The most important option is the model.

The following models are provided (hardcoded configuration):

|Model|Description|
|---|---|
|A500|Amiga 500 with OCS chipset, 0.5MB of RAM and 0.5MB of slow memory expansion|
|A500OG|Amiga 500 with OCS chipset, 0.5MB of RAM|
|A500+|Amiga 500+ with ECS chipset, 1MB of RAM and 1MB of slow memory expansion|
|A600|Amiga 600 with ECS chipset, 2MB of RAM and 8MB of fast memory expansion|
|A1200|Amiga 1200 with AGA chipset, 2MB of RAM and 8MB of fast memory expansion|
|A1200OG|Amiga 1200 with AGA chipset, 2MB of RAM|

As the configuration file is only generated when launching a game you must restart RetroArch for the changes to take effects.

### Kickstarts roms
To use disk and WHDLoad games with this core you'll need the following kickstart roms, rename them to the given name and copy the file to RetroArch system directory.

It is critical to use kickstarts with the right MD5, otherwise the core might not start.

|Name|Description|System|MD5|
|---|---|---|---|
|kick34005.A500|Kickstart v1.3 (Rev. 34.005)|Amiga 500|82a21c1890cae844b3df741f2762d48d|
|kick37175.A500|Kickstart v2.04 (Rev. 37.175)|Amiga 500+|dc10d7bdd1b6f450773dfb558477c230|
|kick40063.A600|Kickstart v3.1 (Rev. 40.063)|Amiga 600|e40a5dfb3d017ba8779faba30cbd1c8e|
|kick40068.A1200|Kickstart v3.1 (Rev. 40.068)|Amiga 1200|646773759326fbac3b2311fd8c8793ee|

### IPF Support
Most full-price commercial Amiga games had some form of custom disk format and/or copy protection on them. For this reason, most commercial Amiga games cannot be stored in ADF files unaltered, but there is an alternative called Interchangeable Preservation Format (IPF) which was specifically designed for this purpose.

IPF support is done through CAPSIMG library. To enable it you have to put a dynamic library called capsimg.dll (Windows) or capsimg.so (Linux, macOS) in your RetroArch system directory.

Compatible CAPSIMG libraries for Windows, macOS and Linux can be found at https://fs-uae.net/download#plugins

Compatible CAPSIMG libraries for Android can be found at https://github.com/rsn8887/capsimg/releases/latest

Please be aware that there are 32-bits and 64-bits versions of the library. Choose the one corresponding to your RetroArch executable.

### M3U Support
When you have a multi disk game, you can use a M3U file to specify each disk of the game and change them from the RetroArch Disk control interface.

A M3U file is a simple text file with one disk per line (see https://en.wikipedia.org/wiki/M3U).

Example:

Simpsons, The - Bart vs. The Space Mutants.m3u
```
Simpsons, The - Bart vs. The Space Mutants_Disk1.adf
Simpsons, The - Bart vs. The Space Mutants_Disk2.adf
```
Path can be absolute or relative to the location of the M3U file.

When a game ask for it, you can change the current disk in the RetroArch 'Disk Control' menu:
- Eject the current disk with 'Disk Cycle Tray Status'.
- Select the right disk index.
- Insert the new disk with 'Disk Cycle Tray Status'.

Note: ZIP support is provided by RetroArch and is done before passing the game to the core. So, when using a M3U file, the specified disk image must be uncompressed (adf, dms, fdi, ipf file formats).

Append "(MD)" as in "MultiDrive" to the M3U filename to insert each disk in different drive for games that support multiple drives. Only possible if there are no more than 4 disks.

### WHDLoad
To use WHDLoad games you'll need to have a prepared WHDLoad image named 'WHDLoad.hdf' in RetroArch system directory.

In this WHDLoad image you must have the three kickstart roms (kick34005.A500, kick40063.A600, kick40068.A1200) in 'Dev/Kickstart' directory.

To do this, you can consult the excellent tutorial made by Allan Lindqvist (http://lindqvist.synology.me/wordpress/?page_id=182) just jump to the 'Create WHDLoad.hdf' section.

The core only support HDD image files format (hdf and hdz) and slave file must be named 'game.Slave'. 

### Create a hdf file for a game 
If you have a WHDLoad game in a zip or a directory, you will have to create an image file.

To do this you can use ADFOpus (http://adfopus.sourceforge.net/) or amitools (https://github.com/cnvogelg/amitools).

Example, to create a hdf file from a zipped WHDLoad game :
- Extract files from the zip to a directory.
- Go to the directory where files were extracted.
- Rename the main slave file (ending with '.Slave') to 'game.Slave' (certains games have many slave files, guess wich is the right one).
- Pack the directory in a hdf file :
	- Using ADFOpus (see [Allan Lindqvist's tutorial](http://lindqvist.synology.me/wordpress/?page_id=182)).
	- Using amitools.
	
The amitools command to use is :
```
xdftool -f <NAME_OF_HDF> pack <GAME_DIRECTORY> size=<SIZE_OF_HDF>
```

Note the size of the HDF specified by SIZE_OF_HDF must be greater than size of the directory to store the additional filesystem informations (I use a 1.25 ratio).

### Game that needs a specific Amiga model (AGA games for instance)
If a game needs a specific Amiga model (AGA games for instance), you can specify which model to use.

To do this just add these strings to your adf, hdf or m3u filename:
- "(A500)" or "(OCS)" to use Amiga 500
- "(A500OG)" or "(512K)" to use Amiga 500 without memory expansion
- "(A500+)" or "(A500PLUS)" to use Amiga 500+
- "(A600)" or "(ECS)" to use Amiga 600
- "(A1200)" or "(AGA)" to use Amiga 1200
- "(A1200OG)" or "(A1200NF)" to use Amiga 1200 without memory expansion
- "(NTSC)" to use NTSC
- "(PAL)" to use PAL
- "(MD)" to insert each disk in different drive (Maximum 4 disks)

Example: When launching "Alien Breed 2 (AGA).hdf" file the core will use an Amiga 1200 model.

If no special string is found the core will use the model configured in core options.

### Resolution and rendering

A said in P-UAE configuration.txt:

```
	To emulate a high-resolution, fully overscanned PAL screen - either
	non-interlaced with line-doubling, or interlaced - you need to use a
	display of at least 720 by 568 pixels. If you specify a smaller size,
	E-UAE's display will be clipped to fit (and you can use the gfx_center_*
	options - see below - to centre the clipped region of the display).
	Similarly, to fully display an over-scanned lo-res PAL screen, you need a
	display of 360 by 284 pixels.
```

Three parameters control the output resolution of the core :

|Name|Values|Default|
|---|---|---|
|Video standard|PAL, NTSC|PAL|
|High resolution|false, true|true|
|Crop overscan|false, true|false|

With this settings all the standards resolutions of the amiga are available :
- **360x284**: PAL Low resolution with overscan
- **320x256**: PAL Low resolution cropped/clipped (without the "borders")
- **360x240**: NTSC Low resolution with overscan
- **320×200**: NTSC Low resolution cropped/clipped (without the "borders")
- **720x568**: PAL High resolution with overscan
- **640×512**: PAL High resolution cropped/clipped (without the "borders")
- **720x480**: NTSC High resolution with overscan
- **640×400**: NTSC High resolution cropped/clipped (without the "borders")

When using a high resolution mode, rendering will be doubled horizontally and vertically for low res games. It's compatible with High res games and the Workbench but scaling shaders (ex: scalefx) will look ugly.

When using a low resolution, scaling shaders (scalefx) looks greats but high res games and and the workbench are badly rendered (but still usable).

## Using a configuration file for your games
You can pass an '.uae' configuration file as a rom and the core will load the settings and start emulation without first showing the gui. 

Look at the sample configuration file "RickDangerous.uae" for help. You can use that sample as a starting point for making your own configuration files for each of your games.

You can find the whole documentation in [configuration.txt](configuration.txt).

Example 1: You want to mount four non-rdb .hdf files. You have one bootable 1000 MB file called `System.hdf` created with surfaces=1, and three non-bootable 2000 MB files called `WHDGamesA.hdf`, `WHDGamesB.hdf`, `WHDGamesC.hdf` created with surfaces=2. Your hdf files are located in the folder with absolute path `/emuroms/amiga/hdf/`. For that scenario, you should create a .uae text file with the following content:
```
hardfile=read-write,32,1,2,512,/emuroms/amiga/hdf/System.hdf
hardfile=read-write,32,2,2,512,/emuroms/amiga/hdf/WHDGamesA.hdf
hardfile=read-write,32,2,2,512,/emuroms/amiga/hdf/WHDGamesB.hdf
hardfile=read-write,32,2,2,512,/emuroms/amiga/hdf/WHDGamesC.hdf
```
You can then load your .uae file via Load Content.

Note that for most hdf files, the model has to be set to A1200 in Quickmenu->Options. This requires a restart to take effect.

If you are using rdb hdf files, please use 0,0,0,0 instead of geometry numbers like 32,1,2,512. The geometry will then be read from the file. This only works for rdb hdf files.

