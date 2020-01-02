# P-UAE LIBRETRO

Based on P-UAE 2.6.1, Git Commit: 0186c1b16f7181ffa02d73e6920d3180ce457c46, Credits to Mustafa 'GnoStiC' TUFAN

All credits to: Richard Drummond "http://www.rcdrummond.net/uae/"

E-UAE is based on the work of dozens of contributors including Bernd
Schmidt (the original author and maintainer of UAE), Bernie Meyer (the
author of the x86 JIT compiler), Toni Wilen (the current maintainer of
WinUAE), and many more.

This port was based at start on PS3 version E-UAE 0.8.29-WIP4 release 8
(so also credits to Ole.)

And of course for the RetroArch/Libretro team: "http://www.libretro.com/"

## Default controls

|Key|Action|
|---|---|
|B|Button 1 / LMB|
|A|Button 2 / RMB|
|L|Toggle virtual keyboard|
|R|Toggle statusbar|

### Virtual keyboard controls
|Key|Action|
|---|---|
|B / Enter|Keypress|
|A|Toggle transparency|
|X|Toggle position|

## Configuration
To generate the temporary uae configuration file the core will use the core options configured in RetroArch.

The most important option is the model.

The following models are provided (hardcoded configuration):

|Model|Description|
|---|---|
|A500|Amiga 500 with OCS chipset, 0.5MB Chip RAM + 0.5MB Slow RAM|
|A500OG|Amiga 500 with OCS chipset, 0.5MB Chip RAM|
|A500+|Amiga 500+ with ECS chipset, 1MB Chip RAM|
|A600|Amiga 600 with ECS chipset, 2MB Chip RAM + 8MB Fast RAM|
|A1200|Amiga 1200 with AGA chipset, 2MB Chip RAM + 8MB Fast RAM|
|A1200OG|Amiga 1200 with AGA chipset, 2MB Chip RAM|
|CD32|Amiga CD32 with AGA chipset, 2MB Chip RAM|

As the configuration file is only generated at launch you must restart RetroArch for the change to take effect.

### Kickstarts ROMs
To use this core you'll need the following Kickstart ROMs. Rename them to the given name and copy the files to RetroArch system directory.

It is critical to use Kickstarts with the correct MD5, otherwise the core might not start.

|System|Description|Filename|Size|MD5|
|---|---|---|---|---|
|Amiga 500|Kickstart v1.3 rev 34.005|**kick34005.A500**|262 144|82a21c1890cae844b3df741f2762d48d|
|Amiga 500+|Kickstart v2.04 rev 37.175|**kick37175.A500**|524 288|dc10d7bdd1b6f450773dfb558477c230|
|Amiga 600|Kickstart v3.1 rev 40.063|**kick40063.A600**|524 288|e40a5dfb3d017ba8779faba30cbd1c8e|
|Amiga 1200|Kickstart v3.1 rev 40.068|**kick40068.A1200**|524 288|646773759326fbac3b2311fd8c8793ee|

For CD32 you need either separate ROMs (Kickstart + extended ROM) or the combined ROM:

|System|Description|Filename|Size|MD5|
|---|---|---|---|---|
|Amiga CD32|CD32 (KS + extended) v3.1 rev 40.060|**kick40060.CD32**|1 048 576|f2f241bf094168cfb9e7805dc2856433|
| **OR** | | | | |
|Amiga CD32|CD32 Kickstart v3.1 rev 40.060|**kick40060.CD32**|524 288|5f8924d013dd57a89cf349f4cdedc6b1|
|Amiga CD32|CD32 Extended ROM rev 40.060|**kick40060.CD32.ext**|524 288|bb72565701b1b6faece07d68ea5da639|


### Resolution and rendering
These parameters control the output resolution of the core:

|Name|Values|Default|
|---|---|---|
|Video Standard|PAL, NTSC|PAL|
|Video Resolution|Lores, Hires (Single line), Hires (Double line)|Hires (Double line)|

With these settings all the standard resolutions are available:

- **720x576** PAL High resolution double line
- **720x288** PAL High resolution single line
- **360x288** PAL Low resolution
- **720x480** NTSC High resolution double line
- **720x240** NTSC High resolution single line
- **360x240** NTSC Low resolution

When using low resolution mode, rendering will be halved horizontally. Scaling shaders looks great but high resolution games and Workbench are badly rendered.

When using high resolution double line mode, rendering will be doubled vertically. It is compatible with high resolution games and Workbench, but scaling shaders will look ugly.

When using high resolution single line mode, rendering is presented as is. It delivers the best of both worlds, and looks great with high resolution games, Workbench and shaders.

## Disk images, WHDLoad and M3U support
You can pass a disk image, a hard drive image, or a playlist file for disk images.

Supported formats are:
- **ADF**, **ADZ**, **IPF**, **DMS**, **FDI** for floppy disk images
- **ISO**, **CUE**, **CCD**, **NRG**, **MDS** for CD images
- **HDF**, **HDZ**, **LHA** for hard drive images
- **M3U** for multiple disk image playlist

When passing these files as a parameter the core will generate a temporary uae configuration file in RetroArch saves directory 
and use it to launch the game.

### Floppy drive sound
For external floppy drive sounds to work, copy the files from https://github.com/libretro/libretro-uae/tree/master/sources/uae_data into a subdirectory called `uae_data` in your RetroArch system directory.

### IPF support
Most full-price commercial Amiga games had some form of custom disk format and/or copy protection on them. For this reason, most commercial Amiga games cannot be stored in ADF files unaltered, but there is an alternative called Interchangeable Preservation Format (IPF) which was specifically designed for this purpose.

IPF support is done through CAPSIMG library. To enable it you have to put a dynamic library called capsimg.dll (Windows) or capsimg.so (Linux, macOS) in your RetroArch system directory.

Compatible CAPSIMG libraries for Windows, macOS and Linux can be found at https://fs-uae.net/download#plugins

Compatible CAPSIMG libraries for Android can be found at https://github.com/rsn8887/capsimg/releases/latest

Please be aware that there are 32-bits and 64-bits versions of the library. Choose the one corresponding to your RetroArch executable.

### M3U support
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
- Eject the current disk with 'Disk Cycle Tray Status'
- Select the right disk index
- Insert the new disk with 'Disk Cycle Tray Status'

Note: ZIP support is provided by RetroArch and is done before passing the game to the core. So, when using a M3U file, the specified disk image must be uncompressed (ADF, DMS, FDI, IPF file formats). ADZ (gzipped ADF) will work though.

Append "(MD)" as in "MultiDrive" to the M3U filename to insert each disk in a different drive for games that support multiple drives. Only possible if there are no more than 4 disks.

## WHDLoad
To use WHDLoad games you'll need to have a prepared WHDLoad image named 'WHDLoad.hdf' in RetroArch system directory.

In this WHDLoad image you must have these Kickstart ROMs (kick34005.A500, kick40068.A1200) in 'Devs:Kickstarts' directory.

To do this, you can consult the excellent tutorial made by Allan Lindqvist (http://lindqvist.synology.me/wordpress/?page_id=182) and just jump to the 'Create WHDLoad.hdf' section.

**NOTE: The tutorial is now outdated regarding default controls and the need of .uae conf files for basic use.**

Grab the new version from the repo: https://github.com/libretro/libretro-uae/tree/master/whdload/

### New WHDLoad.hdf features
- Slave no longer needs to be renamed to game.slave. The first one is selected.
- Kickstarts will be copied automatically from the system directory on the first run, so it might take a little longer than usual.
- `WHDLoad.prefs` will be copied from the system directory, if it exists. It needs to be there for the core option overrides to work.
- `WHDLoad.key` will be copied from the system directory if you have registered WHDLoad.
- **These previous features involving RA system directory require directory filesystem in UAE. At the moment it does not work and will be disabled on these platforms: Android, Switch**
- Supports a file named `custom` in the root of the game.hdf for passing specific WHDLoad parameters when the slave does not support the config screen or when it should be the default, for example `Custom1=1`. It always overrides `WHDLoad.prefs`.
  - The easiest way to create `custom` is to quit WHDLoad (default Numpad*), type `echo custom1=1 >custom`, press enter and reboot the Amiga.
- Supports a file named `load` in the root of the game.hdf which overrides the whole launch command, aimed at non-WHDLoad installs.
- 'Use WHDLoad.hdf' core option does not need to be disabled when launching a non-WHDLoad HDF which has its own startup-sequence.
- NTSC parameter can be used with WHDLoad.
- Included ClickNot for suppressing drive clicking if drive sound emulation is on.
- Included MEmacs for file editing (`custom` & `load`).
- Updated WHDLoad to the latest one (18.5 2019-03-09).
- New WHDLoad defaults:
  - ButtonWait (Waits for a button press in certain slaves when loading is so fast that you can't enjoy a picture or a tune)
  - ReadDelay=0 & WriteDelay=0 (These speed up OS switching on loadings and savings)
- **Latest changes:**
  - Script called `MkCustom` for simplest `custom` file handling. Launches after quitting WHDLoad.
  - If `.slave` is not in the root of the HDF, it will also be searched under the first found directory.
  - Saves can be redirected to a separate `WHDSaves.hdf`. Repo provides an empty 2MiB HDF.
  - Both HDF-files can be located either in RA system or saves.
  - Support for Retroplay LHA archives

### Create a HDF image for a game
If you have a WHDLoad game in a ZIP or a directory, you will have to create an image file. WHDLoad specific LHA archives will work directly as a read only hard drive image.

To do this you can use ADFOpus (http://adfopus.sourceforge.net/) or amitools (https://github.com/cnvogelg/amitools).

Example, to create a HDF file from a zipped WHDLoad game:
- Extract files from the ZIP to a directory
- ~~Go to the directory where files were extracted~~
- ~~Rename the main slave file (ending with '.slave') to 'game.slave' (certains games have many slave files, guess which is the right one)~~
- Pack the directory in a HDF file with:
	- ADFOpus: (see [Allan Lindqvist's tutorial](http://lindqvist.synology.me/wordpress/?page_id=182))
	- amitools: `xdftool -f <NAME_OF_HDF> pack <GAME_DIRECTORY> size=<SIZE_OF_HDF>`
	
Note the size of the HDF specified by SIZE_OF_HDF must be greater than size of the directory to store the additional filesystem informations (f.ex a 1.25 ratio).

## Games that need a specific Amiga model
If a game needs a specific Amiga model (AGA games for instance), you can force the model.

To do this just add the particular string to the filename:

|String|Result|
|---|---|
|**(A500)** or **OCS**|Amiga 500|
|**(A500OG)** or **(512K)**|Amiga 500 without memory expansion|
|**(A500+)** or **(A500PLUS)**|Amiga 500+|
|**(A600)** or **ECS**|Amiga 600|
|**(A1200)** or **AGA**|Amiga 1200|
|**(A1200OG)** or **(A1200NF)**|Amiga 1200 without memory expansion|
|**(NTSC)**|NTSC 60Hz|
|**(PAL)**|PAL 50Hz|
|**(MD)**|Insert each disk in a different drive (Maximum 4 disks)|

Example: When launching "Alien Breed 2 (AGA).hdf" file the core will use the Amiga 1200 model.

If no special string is found the core will use the model configured in the core options. The model core option at 'Automatic' will select A500 when booting floppy disks and A600 when booting hard drives. CD32 will always be selected with CD images.

## Using configuration files
You can pass an '.uae' configuration file and the core will load the settings and start emulation.

Look at the temporary configuration file "puae_libretro.uae" in RetroArch saves as a starting point for your own configuration files.

***Note that the use of configuration files is no longer encouraged or necessary. The core has been modified to always use the core options as a base, so that all custom configurations will be appended to the created configuration, effectively overriding the core options. The problem with this is that changing any core option while the core is running will reset all duplicate configurations. Therefore only add configurations which will require a restart or do not exist in the core options, if you must use a custom uae. If there is an option missing that is a must have, please make an issue about it.***

Example 1: You want to mount four non-RDB HDF files. You have one bootable 1000 MB file called `System.hdf` created with surfaces=1, and three non-bootable 2000 MB files called `WHDGamesA.hdf`, `WHDGamesB.hdf`, `WHDGamesC.hdf` created with surfaces=2. Your hdf files are located in the folder with absolute path `/emuroms/amiga/hdf/`. For that scenario, you should create a .uae text file with the following content:
```
hardfile=read-write,32,1,2,512,/emuroms/amiga/hdf/System.hdf
hardfile=read-write,32,2,2,512,/emuroms/amiga/hdf/WHDGamesA.hdf
hardfile=read-write,32,2,2,512,/emuroms/amiga/hdf/WHDGamesB.hdf
hardfile=read-write,32,2,2,512,/emuroms/amiga/hdf/WHDGamesC.hdf
```
You can then load your .uae file via Load Content.

Note that for most HDF files, the model has to be set to A1200 in Quickmenu->Options. This requires a restart to take effect.

If you are using RDB HDF files, please use 0,0,0,0 instead of geometry numbers like 32,1,2,512. The geometry will then be read from the file. This only works for RDB HDF files.

## Latest features
- CD32 support (sonninnos)
- WHDLoad.prefs override core option (sonninnos)
- Internal+external floppy drive sounds (sonninnos)
- Single line high resolution mode (sonninnos)
- Savestate support (sonninnos)
- Parallel port four player joystick adapter is emulated with working two, three and four-player controls (sonninnos and rsn8887)
- CD32 controller support (sonninnos)
- Multiple mice support using raw input driver on Windows (sonninnos)
- Autozoom and autocentering options for large game display without borders or distortion (sonninnos)
- Mappable hotkeys to bring up statusbar and on-screen keyboard (sonninnos)
- Analog stick mouse control (sonninnos and rsn8887)
- On screen keyboard with easy joystick/dpad control (sonninnos and rsn8887)
- Implement new core options with some small print explanations (sonninnos)
- Implement hotkeys for status bar and virtual keyboard that can be remapped instead of using hardcoded buttons (sonninnos)
- Allow starting the emulated Amiga without content (sonninnos)
- Add many core options, such as a cycle exact option (sonninnos and rsn8887)
- Allow to load any single, bootable .hdf hard drive file directly via load content (rsn8887)
- Allow disabling the use of system/WHDLoad.hdf (rsn8887)
- Fix quickmenu->disk control options for swapping disks on the fly (rsn8887)
- Change option defaults to more sensible values (sonninnos)
- Add Switch version of the core (rsn8887)
- ...
