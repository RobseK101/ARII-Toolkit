#pragma once
#include <ostream>

void createMonitorThread(unsigned int _delayMs, unsigned int _waitMs, size_t _iterationCount, const char* _logfileName, void(*_checkFunction)(std::ostream&));