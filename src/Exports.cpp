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

#include "Exports.h"
#include <Windows.h>
#include <cstring>
#include <cstdio>

#define PATCH_DLLNAME "winmm.dll"

HMODULE originalDll = nullptr;

/* This turned out to be a waste of time:
typedef UINT(WINAPI* PFN_mixerGetNumDevs)(void);
typedef MMRESULT(WINAPI* PFN_mixerGetLineInfoA)(HMIXEROBJ hmxobj, LPMIXERLINEA pmxl, DWORD fdwInfo);
typedef MMRESULT(WINAPI* PFN_mixerClose)(HMIXER hmx);
typedef MMRESULT(WINAPI* PFN_mixerGetLineControlsA)(HMIXEROBJ hmxobj, LPMIXERLINECONTROLSA pmxlc, DWORD fdwControls);
typedef MMRESULT(WINAPI* PFN_mixerOpen)(LPHMIXER phmx, UINT uMxId, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen);
typedef MMRESULT(WINAPI* PFN_mixerGetControlDetailsA)(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails);
typedef MMRESULT(WINAPI* PFN_mixerSetControlDetails)(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails);
typedef MMRESULT(WINAPI* PFN_joyGetDevCapsA)(UINT_PTR uJoyID, LPJOYCAPSA pjc, UINT cbjc);
typedef MMRESULT(WINAPI* PFN_joyGetPosEx)(UINT uJoyID, LPJOYINFOEX pji);
typedef DWORD(WINAPI* PFN_timeGetTime)(void);
*/

void checkAndCommitExport(void** _destination, void* _value, const char* _name)
{
	if (_value == nullptr) {
		char msgBuffer[128] = "Exported function check failed.";
		if (_name) {
			snprintf(msgBuffer, sizeof(msgBuffer), "Exported function \"%s\" was not found.", _name);
		}
		MessageBoxA(NULL, msgBuffer, "Error", MB_OK | MB_SYSTEMMODAL | MB_ICONERROR);
		ExitProcess(1);
	}
	*_destination = _value;
}

void* getAndCheckExport(HMODULE _dll, const char* _name)
{
	void* fnPtr = nullptr;
	checkAndCommitExport(&fnPtr, GetProcAddress(_dll, _name), _name);
	return fnPtr;
}

extern "C" {
	void* real_mixerGetNumDevs = 0;
	void* real_mixerGetLineInfoA = 0;
	void* real_mixerClose = 0;
	void* real_mixerGetLineControlsA = 0;
	void* real_mixerOpen = 0;
	void* real_mixerGetControlDetailsA = 0;
	void* real_mixerSetControlDetails = 0;
	void* real_joyGetDevCapsA = 0;
	void* real_joyGetPosEx = 0;
	void* real_timeGetTime = 0;
	void* real_mmioDescend = 0;
	void* real_mmioCreateChunk = 0;
	void* real_mmioRead = 0;
	void* real_mmioWrite = 0;
	void* real_mmioClose = 0;
	void* real_mmioOpenA = 0;
	void* real_mmioAscend = 0;
	void* real_mciSendCommandW = 0;
	void* real_mciGetErrorStringA = 0;
	void* real_mciSendCommandA = 0;

	__declspec(naked) UINT WINAPI fwd_mixerGetNumDevs(void)
	{
		__asm
		{
			jmp real_mixerGetNumDevs
		}
	}

	__declspec(naked) MMRESULT WINAPI fwd_mixerGetLineInfoA(HMIXEROBJ hmxobj, LPMIXERLINEA pmxl, DWORD fdwInfo)
	{
		__asm
		{
			jmp real_mixerGetLineInfoA
		}
	}

	__declspec(naked) MMRESULT WINAPI fwd_mixerClose(HMIXER hmx)
	{
		__asm
		{
			jmp real_mixerClose
		}
	}

	__declspec(naked) MMRESULT WINAPI fwd_mixerGetLineControlsA(HMIXEROBJ hmxobj, LPMIXERLINECONTROLSA pmxlc, DWORD fdwControls)
	{
		__asm
		{
			jmp real_mixerGetLineControlsA
		}
	}

	__declspec(naked) MMRESULT WINAPI fwd_mixerOpen(LPHMIXER phmx, UINT uMxId, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen)
	{
		__asm
		{
			jmp real_mixerOpen
		}
	}

	__declspec(naked) MMRESULT WINAPI fwd_mixerGetControlDetailsA(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails)
	{
		__asm
		{
			jmp real_mixerGetControlDetailsA
		}
	}

	__declspec(naked) MMRESULT WINAPI fwd_mixerSetControlDetails(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails)
	{
		__asm
		{
			jmp real_mixerSetControlDetails
		}
	}

	__declspec(naked) MMRESULT WINAPI fwd_joyGetDevCapsA(UINT_PTR uJoyID, LPJOYCAPSA pjc, UINT cbjc)
	{
		__asm
		{
			jmp real_joyGetDevCapsA
		}
	}

	__declspec(naked) MMRESULT WINAPI fwd_joyGetPosEx(UINT uJoyID, LPJOYINFOEX pji)
	{
		__asm
		{
			jmp real_joyGetPosEx
		}
	}

	__declspec(naked) DWORD WINAPI fwd_timeGetTime(void)
	{
		__asm
		{
			jmp real_timeGetTime
		}
	}

	__declspec(naked) MMRESULT WINAPI fwd_mmioDescend(HMMIO hmmio, LPMMCKINFO pmmcki, const MMCKINFO* pmmckiParent, UINT fuDescend)
	{
		__asm
		{
			jmp real_mmioDescend
		}
	}

	__declspec(naked) MMRESULT WINAPI fwd_mmioCreateChunk(HMMIO hmmio, LPMMCKINFO pmmcki, UINT fuCreate)
	{
		__asm
		{
			jmp real_mmioCreateChunk
		}
	}

	__declspec(naked) LONG WINAPI fwd_mmioRead(HMMIO hmmio, HPSTR pch, LONG  cch)
	{
		__asm
		{
			jmp real_mmioRead
		}
	}

	__declspec(naked) LONG WINAPI fwd_mmioWrite(HMMIO hmmio, const char _huge* pch, LONG cch)
	{
		__asm
		{
			jmp real_mmioWrite
		}
	}

	__declspec(naked) MMRESULT WINAPI fwd_mmioClose(HMMIO hmmio, UINT fuClose)
	{
		__asm
		{
			jmp real_mmioClose
		}
	}

	__declspec(naked) HMMIO WINAPI fwd_mmioOpenA(LPSTR pszFileName, LPMMIOINFO pmmioinfo, DWORD fdwOpen)
	{
		__asm
		{
			jmp real_mmioOpenA
		}
	}

	__declspec(naked) MMRESULT WINAPI fwd_mmioAscend(HMMIO hmmio, LPMMCKINFO pmmcki, UINT fuAscend)
	{
		__asm
		{
			jmp real_mmioAscend
		}
	}

	__declspec(naked) MCIERROR WINAPI fwd_mciSendCommandW(MCIDEVICEID mciId, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
	{
		__asm
		{
			jmp real_mciSendCommandW
		}
	}

	__declspec(naked) BOOL WINAPI fwd_mciGetErrorStringA(MCIERROR mcierr, LPSTR pszText, UINT cchText) 
	{
		__asm
		{
			jmp real_mciGetErrorStringA
		}
	}

	__declspec(naked) MCIERROR WINAPI fwd_mciSendCommandA(MCIDEVICEID mciId, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2) 
	{
		__asm
		{
			jmp real_mciSendCommandA
		}
	}
}

void __cdecl ForwardExports()
{
	char pathBuffer[512] = {};

	GetSystemDirectoryA(pathBuffer, sizeof(pathBuffer));
	size_t length = strlen(pathBuffer);
	if (length && length + 1 + strlen(PATCH_DLLNAME) >= sizeof(pathBuffer)) {
		MessageBoxA(NULL, "DLL path buffer overflow.", "Patching error", MB_OK | MB_SYSTEMMODAL | MB_ICONERROR);
		ExitProcess(1);
	}
	pathBuffer[length] = '\\';
	strcpy(pathBuffer + length + 1, PATCH_DLLNAME);
	originalDll = LoadLibraryA(pathBuffer);
	if (originalDll == NULL) {
		MessageBoxA(NULL, "Could not find " PATCH_DLLNAME " in the system directory.", "Patching error", MB_OK | MB_SYSTEMMODAL | MB_ICONERROR);
		ExitProcess(1);
	}
	real_mixerGetNumDevs = getAndCheckExport(originalDll, "mixerGetNumDevs");
	real_mixerGetLineInfoA = getAndCheckExport(originalDll, "mixerGetLineInfoA");
	real_mixerClose = getAndCheckExport(originalDll, "mixerClose");
	real_mixerGetLineControlsA = getAndCheckExport(originalDll, "mixerGetLineControlsA");
	real_mixerOpen = getAndCheckExport(originalDll, "mixerOpen");
	real_mixerGetControlDetailsA = getAndCheckExport(originalDll, "mixerGetControlDetailsA");
	real_mixerSetControlDetails = getAndCheckExport(originalDll, "mixerSetControlDetails");
	real_joyGetDevCapsA = getAndCheckExport(originalDll, "joyGetDevCapsA");
	real_joyGetPosEx = getAndCheckExport(originalDll, "joyGetPosEx");
	real_timeGetTime = getAndCheckExport(originalDll, "timeGetTime");
	real_mmioDescend = getAndCheckExport(originalDll, "mmioDescend");
	real_mmioCreateChunk = getAndCheckExport(originalDll, "mmioCreateChunk");
	real_mmioRead = getAndCheckExport(originalDll, "mmioRead");
	real_mmioWrite = getAndCheckExport(originalDll, "mmioWrite");
	real_mmioClose = getAndCheckExport(originalDll, "mmioClose");
	real_mmioOpenA = getAndCheckExport(originalDll, "mmioOpenA");
	real_mmioAscend = getAndCheckExport(originalDll, "mmioAscend");
	real_mciSendCommandW = getAndCheckExport(originalDll, "mciSendCommandW");
	real_mciGetErrorStringA = getAndCheckExport(originalDll, "mciGetErrorStringA");
	real_mciSendCommandA = getAndCheckExport(originalDll, "mciSendCommandA");

#ifdef _DEBUG
	MessageBoxA(NULL, "Completed ForwardExports().", "Debug", MB_OK);
#endif

	return;
}
