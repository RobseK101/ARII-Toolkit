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

#include "AR2Inifile.h"
#include "Inifile.h"
#include <cstring>
#include <algorithm>
#include <string>
#include <vector>
#include <ren/Stringtools.hpp>

namespace AR2Replacement
{
	InifileManager inifileManager(true);
	IniSession* currentIniSession = nullptr;
	std::vector<std::string> onlineArguments;

	void __cdecl setIniFilename(const char* _filename)
	{
		currentIniSession = inifileManager.openIniSession(_filename, true);
	}

	int __cdecl readIniStringSetting(const char* _appName, const char* _keyName, char* _buffer, size_t _bufferSize)
	{
		if (currentIniSession == nullptr) {
			return -1;
		}

		std::string result = currentIniSession->inifile.getValueString(_appName, _keyName);
		size_t copyCount = std::min(result.size(), _bufferSize - 1);
		strncpy(_buffer, result.c_str(), copyCount);
		return copyCount;
	}

	int __cdecl readIniValueSetting(const char* _appName, const char* _keyName)
	{
		if (currentIniSession == nullptr) {
			return -1;
		}

		std::string resultString = currentIniSession->inifile.getValueString(_appName, _keyName);
		return atoi(resultString.c_str());
	}

	bool __cdecl writeIniStringSetting(const char* _appName, const char* _keyName, const char* _source)
	{
		if (currentIniSession == nullptr) {
			return false;
		}

		return currentIniSession->inifile.setValueString(_appName, _keyName, _source);
	}

	bool __cdecl writeIniValueSetting(const char* _appName, const char* _keyName, int _value)
	{
		if (currentIniSession == nullptr) {
			return false;
		}

		return currentIniSession->inifile.setValueString(_appName, _keyName, std::to_string(_value));
	}

	int __cdecl getIniArgumentsOnline(const char* _compoundArgument, const char* _separatorList)
	{
		onlineArguments = ren::st::tokenize(std::string(_compoundArgument), _separatorList, "\t ");
		return onlineArguments.size();
	}

	int __cdecl getIniArgumentValueInt(int _argumentIndex)
	{
		if (_argumentIndex >= 0 && _argumentIndex < onlineArguments.size()) {
			return atoi(onlineArguments[_argumentIndex].c_str());
		}
		else {
			return 0;
		}
	}

	const char* __cdecl getIniArgumentValueStr(int _argumentIndex, char* _buffer, size_t _maxArgumentLength)
	{
		if (_argumentIndex >= 0 && _argumentIndex < onlineArguments.size()) {
			return onlineArguments[_argumentIndex].c_str();
		}
		else {
			return "";
		}
	}
}