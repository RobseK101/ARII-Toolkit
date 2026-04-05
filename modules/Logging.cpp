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

#include "ren/Logging.h"
#include <cstdarg>
#include <cstdio>
#include <ctime>

namespace ren
{
	LogModule* Logging::ps_logModule = nullptr;
	bool Logging::p_forceFlush = false;
	bool Logging::p_doLog = true;
	bool Logging::p_useDebugger = false;

	void Logging::init(LogModule* _module)
	{
		if (ps_logModule) {
			delete ps_logModule;
		}
		ps_logModule = _module;

		time_t timestamp;
		time(&timestamp);

		Logging::log_format("[REN::LOGGING INIT] %s", ctime(&timestamp));
	}

	void __cdecl Logging::log_format(const char* _format, ...)
	{
		va_list args;
		va_start(args, _format);
		char buffer[PRINTBUFFER_SIZE];

		if (ps_logModule && p_doLog) {
			std::vsnprintf(buffer, sizeof(buffer) - 1, _format, args);
			std::string output = buffer;
			ps_logModule->print(output);
			if (p_forceFlush) {
				ps_logModule->flush();
			}
		}
		
		va_end(args);
	}

	void Logging::log_std_string(const std::string& _string)
	{
		if (ps_logModule && p_doLog) {
			ps_logModule->print(_string);
		}
	}

	void Logging::forceFlush(bool _state)
	{
		p_forceFlush = _state;
	}

	void Logging::quit()
	{
		time_t timestamp;
		time(&timestamp);
		p_doLog = true;
		Logging::log_format("[REN::LOGGING QUIT] %s\n", ctime(&timestamp));

		if (ps_logModule) {
			ps_logModule->flush();
			delete ps_logModule;
			ps_logModule = nullptr;
		}
	}

	void Logging::setDebugOptions(int32_t _useDebugger, int32_t _enableLog)
	{
		if (_enableLog) {
			p_doLog = _enableLog;
			p_useDebugger = _useDebugger;
			log_format("[REN::LOGGING enabled (_enableLog = %d, _useDebugger = %d)]\n", _enableLog, _useDebugger);
		}
		else {
			log_format("[REN::LOGGING disabled (_enableLog = %d, _useDebugger = %d)]\n", _enableLog, _useDebugger);
			p_doLog = _enableLog;
			p_useDebugger = _useDebugger;
		}
		return;
	}

	LogfileModule::LogfileModule(const char* _filename) : 
		p_logfile(_filename, std::ios::app), p_buffer() 
	{
		if (!p_logfile.is_open()) {
			
		}
	}

	LogfileModule::~LogfileModule()
	{
		flush();
		p_logfile.close();
	}

	void LogfileModule::print(const std::string& _output)
	{
		p_buffer += _output;
		if (p_buffer.size() >= BUFFER_SIZE) {
			flush();
		}
	}

	void LogfileModule::flush()
	{
		p_logfile << p_buffer;
		p_logfile.flush();
		p_buffer.clear();
	}

	LogModule::~LogModule()
	{
	}

}