#include "Monitor.h"
#include <Windows.h>
#include <fstream>
#include <ctime>
#include <thread>

void monitorProcess(unsigned int _delayMs, unsigned int _waitMs, size_t _iterationCount, const char* _logfileName, void(*_checkFunction)(std::ostream&))
{
    std::ofstream logfile(_logfileName, std::ios::app);
    if (!logfile.is_open()) {
        return;
    }

    time_t currentTime;
    time(&currentTime);

    logfile << "[ARII Toolkit Monitor Thread startup] at " << ctime(&currentTime) << '\n' 
        << "Delay set: " << _delayMs << " ms; waiting interval: " << _waitMs << std::endl;

    Sleep(_delayMs);
    for (size_t i = 0; i < _iterationCount - 1; i++) {
        logfile << "Iteration " << i+1 << '\n';
        _checkFunction(logfile);
        logfile << "\nWaiting for " << _waitMs << " ms..." << std::endl;
        Sleep(_waitMs);
    }
    logfile << "Iteration " << _iterationCount - 1 << '\n';
    _checkFunction(logfile);
    time(&currentTime);
    logfile << "\n[ARII Toolkit Monitor Thread end] at " << ctime(&currentTime) << '\n' << std::endl;
    return;
}

void createMonitorThread(unsigned int _delayMs, unsigned int _waitMs, size_t _iterationCount, const char *_logfileName, void(*_checkFunction)(std::ostream&))
{
    std::thread monitorThread(&monitorProcess, _delayMs, _waitMs, _iterationCount, _logfileName, _checkFunction);
    monitorThread.detach();
    return;
}
