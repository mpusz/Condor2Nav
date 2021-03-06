================
 Condor2Nav 4.0
================

http://sf.net/projects/condor2nav


1. Description
==============
Condor2Nav is making Condor (The Competition Soaring Simulator)and 
gliding navigation applications synchronization easier. It is a fully
automated translator between Condor and navigation applications file
formats.


2. Project state
================
Condor2Nav project is of production quality now. It is stable and
ready to be used. Please report all the problems you possibly encounter
on the forum on the project webpage.


3. Installation
===============
Obtain latest release package from the project website and extract it
anywhere on the hard drive.

The package contains:
 - data/Landscapes/* - LK8000 style template file describing Condor Landscapes
 - data/LK8000/SceneryData.csv - CSV file with supported Condor sceneries information
 - data/XCSoar/SceneryData.csv - CSV file with supported Condor sceneries information
 - data/GliderData.csv - CSV file with all Condor gliders information
 - CHANGELOG.txt - changes introduced in subsequent releases
 - condor2nav.ini - project configuration
 - condor2nav-cli.exe - CLI application executable
 - condor2nav-gui.exe - GUI application executable
 - QuickStart-LK8000.txt - quick start guide for LK8000 users
 - QuickStart-XCSoar.txt - quick start guide for XCSoar users
 - README.txt - that readme file


4. Additional software
======================
To run the translation you will need to have Condor (The Competition
Soaring Simulator) on your local computer.

4.1.a) XCSoar
-------------
In case of XCSoar translation following steps must be completed:
1. Have XCSoar installed and configured on the target device
   (e.g. PDA, PNA, PC).
2. Obtain XCM map files for all supported sceneries from condor2nav
   project webpage
   (http://sourceforge.net/projects/condor2nav/files/maps/XCSoar/Maps.zip/download)
3. Find "XCSoarData" directory created with XCSoar and copy all
   XCM scenery files to the [XCSoar] "Condor2NavDataSubDir" subdirectory
   on the target device.
4. [OPTIONAL] If [Condor2Nav] "OutputPath" is different than the
   [XCSoar] "XCSoarDataPath" copy "[XCSOAR_DATA_PATH]/xcsoar-registry.prf"
   file to the "[CONDOR_2_NAV_PATH]/data" directory.

4.2.b) LK8000
-------------
In case of LK8000 translation following steps must be completed:
1. Have LK8000 installed and configured on the target device
   (e.g. PDA, PNA, PC).
2. Copy directory "[CONDOR_2_NAV_PATH]/data/LK8000/_Waypoints" with waypoints for all
   supported Condor landscapes to "LK8000" subdirectory on the target device with
   your LK8000 instalation.
3. If [LK8000] "CheckForMapUpdates=1" Condro2Nav will check on the startup for
   new better maps matching Condor Landscapes. If such maps are available
   they will be downloaded automatically.
4. Copy "[CONDOR_2_NAV_PATH]/data/LK8000/_Maps" subdirectory containing all
   downloaded LK8000 maps to "LK8000" directory.
   subdirectory on the target device with LK8000 instalation.
5. [OPTIONAL] If [Condor2Nav] "OutputPath" is different than the [LK8000]
   "LK8000Path" copy "[LK8000_DATA_PATH]/_Configuration/DEFAULT_PROFILE.prf"
   file to the "[CONDOR_2_NAV_PATH]/data" directory.


5. Configuration
================
Condor2Nav is preconfigured to make it work with no issues on any platform
and at the same time require as small effort for every Condor task
translation as possible. When using default configuration, below steps marked
as [OPTIONAL] may be skipped.
However it is possible to configure it in the other way maybe to better
suite user's needs. In that case [OPTIONAL] points may be useful.

Open condor2nav.ini file and change the following:
1. [OPTIONAL] Change the name of Condor default task file you will use for
   the translation (Condor task file name can also be provided for each
   translation from the Command Line Interface - see "condor2nav-cli.exe -h"
   for more info).
2. [OPTIONAL] If Condor FPL files are stored in non default location
   please use [Condor] "FlightPlansPath" and [Condor] "RaceResultsPath"
   parameters to provide correct paths.
3. [OPTIONAL] Condor2Nav is configured to use ActiveSync connection by
   default. In case of other translation target (local directory, connected
   flash card) change the application output path ([Condor2Nav] "OutputPath").
4. [OPTIONAL] In [Condor2Nav] chapter setup which data should be translated.
5. [OPTIONAL] In case of XCSoar translation and if proposed Condor2Nav data
   path on the target device doesn't suite your needs change
   [XCSoar] "Condor2NavDataSubDir".
6. [OPTIONAL] In case of LK8000 translation you can enable
   [LK8000] "DefaultProfilesOverwrite=1" option to not have to choose Condor
   secific profiles of each LK8000 startup. Please backup your working
   profile before doing that!!!


6. Translation
==============
6.1. Translation Types
----------------------
6.1.a) Default task
-------------------
1. Open Condor and save the task for translation. By default "A" task name
   is used for translation.
2. [OPTIONAL] In case of any other Condor task name, condor2nav.ini file 
   should be updated.

6.1.b) Last Race
----------------
1. It is possible to translate Condor task even if saving it on the disk is
   not allowed by the server. To do that user should join Condor multiplayer
   race and right after that quit the game. Now Condor2Nav is able to
   translate the data when it will be run with 'Last Race' option.

6.1.c) Other task
-----------------
1. For GUI version just select desired file after clicking "Other" radio button.
   For CLI version provide task file full path to Command Line Interface 
   (see "condor2nav-cli.exe -h" for more info) or a task file icon should
   be drag-and-dropped on the condor2nav-cli.exe icon.
   
6.2. Translate
--------------
1. Run condor2nav-gui.exe or condor2nav-cli.exe Condor2Nav application
   (it is handful to create a shortcut to application executable on
   the Windows desktop).

6.3.a) XCSoar
-------------
2. [OPTIONAL] If [Condor2Nav] "OutputPath" is different than the
   [XCSoar] "XCSoarDataPath" copy output data manually to the target directory.

6.3.b) LK8000
-------------
2. [OPTIONAL] If [Condor2Nav] "OutputPath" is different than the
   [LK8000] "LK8000Path" copy output data manually to the target directory.
   
7. Fly
======

7.1.a) XCSoar
-------------
1. Start XCSoar application and select "Condor" profile when asked.
2. [OPTIONAL] If [XCSoar] "DefaultTaskOverwrite=1" XCSoar will load Condor
   task automatically. Otherwise open "Nav"->"Nav"->"Task Edit",
   choose "Browse", select "Condor.tsk" and selct "Load".
3. Open "Config"->"Flight Setup" and choose proper "Ballast" value.
   
7.1.b) LK8000
-------------
1. Start LK8000 application and select "Condor" profile when asked.
2. [OPTIONAL] If [LK8000] "DefaultTaskOverwrite=1" LK8000 will load Condor
   task automatically. Otherwise open "Nav"->"Nav"->"Task Edit",
   choose "Load" and select "Condor.tsk".
3. Open "Config"->"Setup Basic" and choose proper "Ballast" value.

7.2. Have fun
-------------
You are now ready to fly.
