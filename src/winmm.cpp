/*
MIT License

Copyright (c) 2026 Dr. R. Klenk (a.k.a. RobseK, RobseK101)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <Windows.h>
#include <cstring>
#include "Exports.h"
#include "spel.h"
#include <ren/Logging.h>
#include "AR2Inifile.h"

// Big thanks to "Silent" for releasing his ASI Loader as open source and therefore providing a nice way 
// to patch a binary.

static_assert(sizeof(intptr_t) == 4, "This module may only be compiled for x86 mode.");

#define PATCH_DLLNAME "winmm.dll"

struct TrampolineBackup
{
	BYTE* address;
	BYTE codeBackup[5];
};

BYTE originalEntryPointCode[5] = {};
BYTE* originalEntryPoint = nullptr;
HINSTANCE hExecutableInstance = 0; // basically the address of the module (the exe file) in virtual memory.
BYTE* moduleBaseAddress = 0; // renamed copy for clarity

TrampolineBackup entryPointBackup = {};

char strbuf256[256] = {"(c) 2026 RobseK"};

void __cdecl cdeclNull() { return; }
void __stdcall stdcallNull() { return; }

DWORD PatchMovR32Imm32(void* _instruction, DWORD _newValue)
{
	// Opcode B8, 5 bytes
	BYTE* instruction = reinterpret_cast<BYTE*>(_instruction);

	DWORD oldProtect;
	VirtualProtect(_instruction, 5, PAGE_EXECUTE_READWRITE, &oldProtect);

	DWORD oldValue;
	memcpy(&oldValue, instruction + 1, sizeof(DWORD));
	memcpy(instruction + 1, &_newValue, sizeof(DWORD));

	FlushInstructionCache(GetCurrentProcess(), _instruction, 5);

	DWORD dummy;
	VirtualProtect(_instruction, 5, oldProtect, &dummy);

	return oldValue;
}

TrampolineBackup InstallOneWayTrampoline(void* _location, void* _destination)
{
	BYTE* location = reinterpret_cast<BYTE*>(_location);

	// Unprotect the target location:
	DWORD oldProtect;
	VirtualProtect(_location, 5, PAGE_EXECUTE_READWRITE, &oldProtect);

	// Backup the original code and address:
	TrampolineBackup backup;
	backup.address = location;
	memcpy(backup.codeBackup, location, 5);

	// Patch to call our own entry point:
	ptrdiff_t destinationOffset = reinterpret_cast<intptr_t>(_destination) - (reinterpret_cast<intptr_t>(location) + 5);
	location[0] = 0xE9;
	memcpy(location + 1, &destinationOffset, sizeof(destinationOffset));

	FlushInstructionCache(GetCurrentProcess(), _location, 5);

	// Restore the protection:
	DWORD dummy;
	VirtualProtect(_location, 5, oldProtect, &dummy);

	return backup;
}

void UninstallOneWayTrampoline(TrampolineBackup _backup)
{
	DWORD oldProtect;
	VirtualProtect(_backup.address, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(_backup.address, _backup.codeBackup, 5);

	FlushInstructionCache(GetCurrentProcess(), _backup.address, 5);

	DWORD dummy;
	VirtualProtect(_backup.address, 5, oldProtect, &dummy);
	return;
}

void ApplyPatches()
{
	// Do stuff:
	ren::Logging::init(new ren::LogfileModule("ren_debug.txt"));

	// Replaces the logger with the new one
	InstallOneWayTrampoline(moduleBaseAddress + Offsets::Debug__setDebugOptions, &ren::Logging::setDebugOptions);
	InstallOneWayTrampoline(moduleBaseAddress + Offsets::Debug__log, &ren::Logging::log_format);

	// Removes CD checking
	InstallOneWayTrampoline(moduleBaseAddress + Offsets::FileIO__check_file_on_cdrom, &cdeclNull);
	InstallOneWayTrampoline(moduleBaseAddress + Offsets::FileIO__check_sec_file, &cdeclNull);

	// Fixes the startup crash bug
	PatchMovR32Imm32(moduleBaseAddress + Offsets::initPlayerCarMov1, reinterpret_cast<DWORD>(&strbuf256));
	PatchMovR32Imm32(moduleBaseAddress + Offsets::initPlayerCarMov2, reinterpret_cast<DWORD>(&strbuf256));
	PatchMovR32Imm32(moduleBaseAddress + Offsets::initPlayerCarMov3, reinterpret_cast<DWORD>(&strbuf256));
	PatchMovR32Imm32(moduleBaseAddress + Offsets::initPlayerCarMov4, reinterpret_cast<DWORD>(&strbuf256));
	PatchMovR32Imm32(moduleBaseAddress + Offsets::initPlayerCarMov5, reinterpret_cast<DWORD>(&strbuf256));
	PatchMovR32Imm32(moduleBaseAddress + Offsets::initPlayerCarMov6, reinterpret_cast<DWORD>(&strbuf256));

	// Replaces the INI handling facilites
	InstallOneWayTrampoline(moduleBaseAddress + Offsets::Inifile__get_ini_argument_value_int, &AR2Replacement::getIniArgumentValueInt);
	InstallOneWayTrampoline(moduleBaseAddress + Offsets::Inifile__get_ini_argument_value_str, &AR2Replacement::getIniArgumentValueStr);
	InstallOneWayTrampoline(moduleBaseAddress + Offsets::Inifile__get_ini_arguments_online, &AR2Replacement::getIniArgumentsOnline);
	InstallOneWayTrampoline(moduleBaseAddress + Offsets::Inifile__read_ini_string_setting, &AR2Replacement::readIniStringSetting);
	InstallOneWayTrampoline(moduleBaseAddress + Offsets::Inifile__read_ini_value_setting, &AR2Replacement::readIniValueSetting);
	InstallOneWayTrampoline(moduleBaseAddress + Offsets::Inifile__set_ini_filename, &AR2Replacement::setIniFilename);
	InstallOneWayTrampoline(moduleBaseAddress + Offsets::Inifile__write_ini_string_setting, &AR2Replacement::writeIniStringSetting);
	InstallOneWayTrampoline(moduleBaseAddress + Offsets::Inifile__write_ini_value_setting, &AR2Replacement::writeIniValueSetting);

	MessageBoxA(NULL, "Completed ApplyPatches().", "Debug", MB_OK);
	return;
}

void* RestoreEntryPoint()
{
	/*
	memcpy(originalEntryPoint, originalEntryPointCode, sizeof(originalEntryPointCode));
	*/
	UninstallOneWayTrampoline(entryPointBackup);
	MessageBoxA(NULL, "Completed RestoreEntryPoint().", "Debug", MB_OK);
	return entryPointBackup.address;
}

void __declspec(naked) Main_DoInit()
{
	_asm
	{
		call ForwardExports
		call ApplyPatches
		call RestoreEntryPoint
		jmp eax
	}

	/*_asm
	{
		call ForwardExports
		call ApplyPatches
		call RestoreEntryPoint
		jmp originalEntryPoint
	}*/
}

// Main DLL entry point:
BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason) {
		case DLL_PROCESS_ATTACH:
		{
			// Get the module's (i.e. the exe file's) address in virtual memory:
			hExecutableInstance = GetModuleHandleA(NULL);

			// Unused; seems to be a remnant of the copied code by Silent.
			char buffer[512] = {};
			GetModuleFileNameA(hExecutableInstance, buffer, sizeof(buffer));

			if (hExecutableInstance) {
				moduleBaseAddress = reinterpret_cast<BYTE*>(hExecutableInstance);

				// Get the entry point location:
				IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(moduleBaseAddress);
				IMAGE_NT_HEADERS* ntHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(reinterpret_cast<DWORD>(moduleBaseAddress) + dosHeader->e_lfanew);
				BYTE* entryPoint = reinterpret_cast<BYTE*>(reinterpret_cast<DWORD>(moduleBaseAddress) + ntHeader->OptionalHeader.AddressOfEntryPoint);

				/*
				// Unprotect the entry point:
				DWORD oldProtect;
				VirtualProtect(entryPoint, 5, PAGE_EXECUTE_READWRITE, &oldProtect);

				// back up the original code
				memcpy(originalEntryPointCode, entryPoint, sizeof(originalEntryPointCode));

				// Patch to call our own entry point:
				int newEntryPoint = reinterpret_cast<int>(&Main_DoInit) - (reinterpret_cast<int>(entryPoint) + 5);
				entryPoint[0] = 0xE9;
				memcpy(entryPoint + 1, &newEntryPoint, sizeof(originalEntryPointCode) - 1);

				originalEntryPoint = entryPoint;
				*/

				entryPointBackup = InstallOneWayTrampoline(entryPoint, &Main_DoInit);
			}
			break;
		}
		case DLL_THREAD_ATTACH:
		{
			break;
		}
		case DLL_THREAD_DETACH:
		{
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			ren::Logging::quit();
			break;
		}
	}
	return true;
}