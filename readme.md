# ARII Toolkit

This is a collection of tools aimed at making the classic game "Autobahn Raser II" operable again on modern operating systems. It also includes utilites to help reversing the game's code, which is an additional aim of this project.

Right now, this project compiles down to two usable files:

- winmm.dll: The DLL hook that is inserted into the Autobahn Raser II directory and is loaded on start of spel.dat/spel.exe.
  Note that right now, this file *does not discriminate* by which program it was loaded. Therefore you will need to rename spel.dat to spel.exe and run that directly - otherwise you will run a corrupted instance of menu.exe. This will be fixed. 
  **Note:** You must specify x86/Win32 as the compilation target in CMake. Otherwise the DLL hook will not work. 

- InifileTest.exe: A utility to test the Inifile module contained in this project. 

## Issues addressed by the DLL hook at the moment

- The game crashing during loading on any OS newer than Windows Me -> fixed
- Requirement to insert the game CD -> removed
- *Very* slow logging functionality (accessible through setting the **DEBUGON** key in the **HELPDESK** section of raser.ini) -> fixed

## Other implemented features

- An INI file reader/writer that bypasses the outdated GetPrivateProfile... and WritePrivateProfile... set of Windows API functions. 
  This creates the possibility of INI value overrides.