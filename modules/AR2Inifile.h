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

namespace AR2Replacement
{
	void __cdecl setIniFilename(const char* _filename);

	int __cdecl readIniStringSetting(const char* _appName, const char* _keyName, char* _buffer, size_t _bufferSize); // Return value is count of copied chars
	int __cdecl readIniValueSetting(const char* _appName, const char* _keyName);

	bool __cdecl writeIniStringSetting(const char* _appName, const char* _keyName, const char* _source);
	bool __cdecl writeIniValueSetting(const char* _appName, const char* _keyName, int _value);

	int __cdecl getIniArgumentsOnline(const char* _compoundArgument, const char* _separatorList);
	int __cdecl getIniArgumentValueInt(int _argumentIndex);
	char* __cdecl getIniArgumentValueStr(int _argumentIndex, char* _buffer, size_t _maxArgumentLength); // _maxArgumentLength passed to strncpy 
}