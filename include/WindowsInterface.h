#pragma once
#include <chrono>
#include <filesystem>
#include <string>
#include <utility>

bool setFiledates(const std::wstring& filepath, std::chrono::year_month_day date, unsigned hour = 3);

std::string getCommandLine();
std::wstring getCommandLineW();

// Note that command line applications expect the exe path to be included as the first 
// item in the cmd line. This is *not* handled automatically!
int createAndWaitProcess(const std::string& exePath, const std::string& cmdLine);

// Note that command line applications expect the exe path to be included as the first 
// item in the cmd line. This is *not* handled automatically!
int createAndWaitProcess(const std::wstring& exePath, const std::wstring& cmdLine);

// Note that command line applications expect the exe path to be included as the first 
// item in the cmd line. This is *not* handled automatically!
std::pair<int, std::string> createAndWaitProcessSilent(const std::string& exePath, const std::string& cmdLine);

// Note that command line applications expect the exe path to be included as the first 
// item in the cmd line. This is *not* handled automatically!
std::pair<int, std::string> createAndWaitProcessSilent(const std::wstring& exePath, const std::wstring& cmdLine);