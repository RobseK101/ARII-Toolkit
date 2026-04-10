#pragma once
#include <cstdarg>
#include <cstdio>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#define REN_NORETURN __declspec(noreturn)
#else
#define REN_NORETURN __attribute__ ((noreturn))
#endif // _WIN32

/// @file
/// @brief Exception helper functions.

namespace ren
{
    inline void (*exceptionsLogFn)(const char*, ...) = nullptr;
    inline void (*exceptionsLogAbortFn)(void) = nullptr;

    /// @brief Throws an exception. The string buffer for this operation is 512 bytes.
    /// @tparam E The exception type; descendant of std::exception.
    /// @param format Format string using the printf() syntax.
    /// @param ... The format parameters.
    template<class E>
    [[noreturn]] inline void throwException(const char* format ...)
    {
        va_list args;
        va_start(args, format);
        char buffer[512] {};
        std::vsnprintf(buffer, sizeof(buffer) - 1, format, args);
        va_end(args);

        if (exceptionsLogFn) {
            exceptionsLogFn("[EXCEPTION] %s\n", buffer);
        }
        throw E(buffer);
    }

    /// @brief Throws an std::runtime_error with the message "Error code = [errorCode]".
    /// @param errorCode 
    [[noreturn]] inline void throwException(int errorCode)
    {
        throwException<std::runtime_error>("Error code = %d", errorCode);
    }

    /// @brief Throws an exception.
    /// @tparam E The exception type; descendant of std::exception.
    /// @param message The exception message.
    template<class E>
    [[noreturn]] inline void throwException(const std::string& message)
    {
        if (exceptionsLogFn) {
            exceptionsLogFn("[EXCEPTION] %s\n", message.c_str());
        }
        throw E(message);
    }
}
