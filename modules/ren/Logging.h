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
#include <cstring>
#include <string>
#include <fstream>
#include <cstdint>

/// @file
/// @brief Logging utilities

namespace ren
{
	// Interface
	class LogModule
	{
	public:
		virtual ~LogModule();
		virtual void print(const std::string& _output) = 0;
		virtual void flush() = 0;
	};

	// "static" class
	class Logging
	{
	public:
		static constexpr size_t PRINTBUFFER_SIZE = 512;

		static void init(LogModule* _module);
		static void __cdecl log_format(const char* _format, ...);
		static void log_std_string(const std::string& _string);
		static void forceFlush(bool _state);
		static void quit();

		static void __cdecl setDebugOptions(int32_t _useDebugger, int32_t _enableLog);

	private:
		static LogModule* ps_logModule;
		static bool p_forceFlush;
		static bool p_doLog;
		static bool p_useDebugger;
	};

	class LogfileModule : public LogModule
	{
	public:
		static constexpr size_t BUFFER_SIZE = 1024 * 4;

		LogfileModule() = delete;
		LogfileModule(const char* _filename);
		LogfileModule(const LogfileModule&) = delete;
		LogfileModule(LogfileModule&&) = delete;

		virtual ~LogfileModule() override;
		virtual void print(const std::string& _output) override;
		virtual void flush() override;

	private:
		std::string p_buffer;
		std::ofstream p_logfile;
	};
}