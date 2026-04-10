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

#pragma once
#include <cstdint>
#include <Windows.h>

constexpr IMAGE_FILE_HEADER SpelDatImageHeader = {
    332,
    3,
    938094720,
    0,
    0,
    224,
    271
};

namespace Offsets
{
	constexpr intptr_t imageBaseAddress = 0x00400000;

	constexpr intptr_t Debug__setDebugOptions = 0x00463B20 - imageBaseAddress;
	constexpr intptr_t Debug__log = 0x00463B40 - imageBaseAddress;

	constexpr intptr_t FileIO__check_file_on_cdrom = 0x0046CAA0 - imageBaseAddress;
	constexpr intptr_t FileIO__check_sec_file = 0x0042E2B0 - imageBaseAddress;

	constexpr intptr_t Inifile__get_ini_argument_value_int = 0x00465650 - imageBaseAddress;
	constexpr intptr_t Inifile__get_ini_argument_value_str = 0x004656A0 - imageBaseAddress;
	constexpr intptr_t Inifile__get_ini_arguments_online = 0x004654F0 - imageBaseAddress;
	constexpr intptr_t Inifile__read_ini_string_setting = 0x004654A0 - imageBaseAddress;
	constexpr intptr_t Inifile__read_ini_value_setting = 0x00465460 - imageBaseAddress;
	constexpr intptr_t Inifile__set_ini_filename = 0x004652F0 - imageBaseAddress;
	constexpr intptr_t Inifile__write_ini_string_setting = 0x004653C0 - imageBaseAddress;
	constexpr intptr_t Inifile__write_ini_value_setting = 0x00465400 - imageBaseAddress;

	constexpr intptr_t initPlayerCarMov1 = 0x004334AE - imageBaseAddress;
	constexpr intptr_t initPlayerCarMov2 = 0x004334D0 - imageBaseAddress;
	constexpr intptr_t initPlayerCarMov3 = 0x0043350D - imageBaseAddress;
	constexpr intptr_t initPlayerCarMov4 = 0x00433532 - imageBaseAddress;
	constexpr intptr_t initPlayerCarMov5 = 0x00433554 - imageBaseAddress;
	constexpr intptr_t initPlayerCarMov6 = 0x00433592 - imageBaseAddress;

	constexpr intptr_t hWnd_global = 0x0050B77C - imageBaseAddress;
	constexpr intptr_t g_wndProcFunctions = 0x00510DB4 - imageBaseAddress;
}

namespace Session
{
    void init();

    extern intptr_t imageBaseAddress;
    extern char moduleFilepath[512];
    extern char moduleFilename[256];
}

struct WndProcFunctions
{
    void *ptr_WM_COMMAND_func;
    void *ptr_WM_CHAR_func;
    void *ptr_WM_SYSCHAR_func;
    void *ptr_WM_SYSKEYDOWN_func;
    void *ptr_WM_MOUSEMOVE_func;
    void *ptr_WM_LBUTTONDOWN_func;
    void *ptr_WM_LBUTTONUP_func;
    void *ptr_WM_RBUTTONDOWN_func;
    void *ptr_WM_RBUTTONUP_func;
    void *ptr_WM_KILLFOCUS_func;
    void *ptr_WM_SETFOCUS_func;
    void *ptr_WM_SETCURSOR_func;
    void *ptr_WM_PAINT_func;
    void *ptr_WM_KEYDOWN_func;
    void *ptr_WM_INITMENU_func;
    void *ptr_WM_MENUSELECT_func;
    void *ptr_WM_DRAWITEM_func;
    void *ptr_WM_MEASUREITEM_func;
    void *ptr_WM_PALETTECHANGED_func;
    void *ptr_WM_QUERYNEWPALETTE_func;
    void *ptr_WM_CTLCOLORBTN_func;
    void *ptr_WM_CTLCOLORDLG_func;
    void *ptr_WM_CTLCOLOREDIT_func;
    void *ptr_WM_CTLCOLORLISTBOX_func;
    void *ptr_WM_CTLCOLORMSGBOX_func;
    void *ptr_WM_CTLCOLORSCROLLBAR_func;
    void *ptr_WM_COLORSTATIC_func;
};

#define $hWnd_global (*reinterpret_cast<HWND*>(Session::imageBaseAddress + Offsets::hWnd_global))
#define $g_wndProcFunctions (*reinterpret_cast<WndProcFunctions*>(Session::imageBaseAddress + Offsets::g_wndProcFunctions))