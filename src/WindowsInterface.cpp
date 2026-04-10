#include "WindowsInterface.h"
#include <string>
#include <Windows.h>
#include <ren/Exceptions.hpp>
#include <vector>
#include <ren/Stringtools.hpp>

bool setFiledates(const std::wstring& filepath, std::chrono::year_month_day date, unsigned hour)
{
	SYSTEMTIME systime = {};
	systime.wYear = (int)date.year();
	systime.wMonth = (unsigned)date.month();
	systime.wDay = (unsigned)date.day();
	systime.wHour = hour;

	FILETIME filetime;
	if (!SystemTimeToFileTime(&systime, &filetime)) {
		return false;
	}

	HANDLE hFile = CreateFileW(
		filepath.c_str(),
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);
	if (hFile == INVALID_HANDLE_VALUE) {
		return false;
	}

	bool filetimeResult = SetFileTime(hFile, &filetime, &filetime, &filetime);

	CloseHandle(hFile);
	return filetimeResult;
}

std::string getCommandLine()
{
	return GetCommandLineA();
}

std::wstring getCommandLineW()
{
	return GetCommandLineW();
}

int createAndWaitProcess(const std::string& exePath, const std::string& cmdLine)
{
	STARTUPINFOA startupInfo = { sizeof(startupInfo) };
	PROCESS_INFORMATION processInformation = {};
	char* cmdLineBuffer = new char[cmdLine.size() + 1] {};
	std::memcpy(cmdLineBuffer, cmdLine.data(), cmdLine.size());

	bool createProcessSuccess = CreateProcessA(
		exePath.c_str(),
		cmdLineBuffer,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&startupInfo,
		&processInformation
	);

	if (!createProcessSuccess) {
		ren::throwException<std::runtime_error>("%s(): Failed to create process; Error code: %d", __FUNCTION__, GetLastError());
	}

	WaitForSingleObject(processInformation.hProcess, INFINITE);

	DWORD exitCode;
	if (!GetExitCodeProcess(processInformation.hProcess, &exitCode)) {
		ren::throwException<std::runtime_error>("%s(): Failed to retrieve exit code; Error code: &d", __FUNCTION__, GetLastError());
	}

	CloseHandle(processInformation.hThread);
	CloseHandle(processInformation.hProcess);
	delete[] cmdLineBuffer;

	return exitCode;
}

int createAndWaitProcess(const std::wstring& exePath, const std::wstring& cmdLine)
{
	STARTUPINFOW startupInfo = { sizeof(startupInfo) };
	PROCESS_INFORMATION processInformation = {};
	wchar_t* cmdLineBuffer = new wchar_t[cmdLine.size() + 1] {};
	std::memcpy(cmdLineBuffer, cmdLine.data(), cmdLine.size() * sizeof(wchar_t));

	bool createProcessSuccess = CreateProcessW(
		exePath.c_str(),
		cmdLineBuffer,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&startupInfo,
		&processInformation
	);

	if (!createProcessSuccess) {
		ren::throwException<std::runtime_error>("%s(): Failed to create process; Error code: %d", __FUNCTION__, GetLastError());
	}

	WaitForSingleObject(processInformation.hProcess, INFINITE);

	DWORD exitCode;
	if (!GetExitCodeProcess(processInformation.hProcess, &exitCode)) {
		ren::throwException<std::runtime_error>("%s(): Failed to retrieve exit code; Error code: &d", __FUNCTION__, GetLastError());
	}

	CloseHandle(processInformation.hThread);
	CloseHandle(processInformation.hProcess);
	delete[] cmdLineBuffer;

	return exitCode;
}

std::pair<int, std::string> createAndWaitProcessSilent(const std::string& exePath, const std::string& cmdLine)
{
	std::string output;

	SECURITY_ATTRIBUTES securityAttributes = { sizeof(securityAttributes) };
	securityAttributes.bInheritHandle = TRUE;
	securityAttributes.lpSecurityDescriptor = nullptr;

	HANDLE hRead, hWrite;
	bool createPipeSuccess = CreatePipe(&hRead, &hWrite, &securityAttributes, 0);
	if (!createPipeSuccess) {
		ren::throwException<std::runtime_error>("%s(): Failed to create output pipe; Error code: %d", __FUNCTION__, GetLastError());
	}

	SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);

	STARTUPINFOA startupInfo = { sizeof(startupInfo) };
	startupInfo.dwFlags = STARTF_USESTDHANDLES;
	startupInfo.hStdOutput = hWrite;
	startupInfo.hStdError = hWrite;

	PROCESS_INFORMATION	processInfo = {};

	std::vector<char> cmdLineBuffer(cmdLine.size() + 1, 0);
	std::memcpy(cmdLineBuffer.data(), cmdLine.c_str(), cmdLine.size());

	bool createProcessSuccess = CreateProcessA(
		exePath.c_str(),
		cmdLineBuffer.data(),
		NULL,
		NULL,
		TRUE,
		CREATE_NO_WINDOW,
		NULL,
		NULL,
		&startupInfo,
		&processInfo
	);

	if (!createProcessSuccess) {
		CloseHandle(hRead);
		CloseHandle(hWrite);
		ren::throwException<std::runtime_error>("%s(): Failed to create process; Error code: %d", __FUNCTION__, GetLastError());
	}

	WaitForSingleObject(processInfo.hProcess, INFINITE);
	CloseHandle(hWrite);

	char buffer[4096];
	DWORD bytesRead;
	while (ReadFile(hRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
		buffer[bytesRead] = 0;
		output += buffer;
	}

	CloseHandle(hRead);

	DWORD exitCode;
	if (!GetExitCodeProcess(processInfo.hProcess, &exitCode)) {
		ren::throwException<std::runtime_error>("%s(): Failed to retrieve exit code; Error code: &d", __FUNCTION__, GetLastError());
	}

	std::pair<int, std::string> result = { exitCode, std::move(output) };

	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);

	return result;
}

std::pair<int, std::string> createAndWaitProcessSilent(const std::wstring& exePath, const std::wstring& cmdLine)
{
	std::string output;

	SECURITY_ATTRIBUTES securityAttributes = { sizeof(securityAttributes) };
	securityAttributes.bInheritHandle = TRUE;
	securityAttributes.lpSecurityDescriptor = nullptr;

	HANDLE hRead, hWrite;
	bool createPipeSuccess = CreatePipe(&hRead, &hWrite, &securityAttributes, 0);
	if (!createPipeSuccess) {
		ren::throwException<std::runtime_error>("%s(): Failed to create output pipe; Error code: %d", __FUNCTION__, GetLastError());
	}

	SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);

	STARTUPINFOW startupInfo = { sizeof(startupInfo) };
	startupInfo.dwFlags = STARTF_USESTDHANDLES;
	startupInfo.hStdOutput = hWrite;
	startupInfo.hStdError = hWrite;

	PROCESS_INFORMATION	processInfo = {};

	std::vector<wchar_t> cmdLineBuffer(cmdLine.size() + 1, 0);
	std::memcpy(cmdLineBuffer.data(), cmdLine.c_str(), cmdLine.size() * sizeof(wchar_t));

	bool createProcessSuccess = CreateProcessW(
		exePath.c_str(),
		cmdLineBuffer.data(),
		NULL,
		NULL,
		TRUE,
		CREATE_NO_WINDOW,
		NULL,
		NULL,
		&startupInfo,
		&processInfo
	);

	if (!createProcessSuccess) {
		CloseHandle(hRead);
		CloseHandle(hWrite);
		ren::throwException<std::runtime_error>("%s(): Failed to create process; Error code: %d", __FUNCTION__, GetLastError());
	}

	WaitForSingleObject(processInfo.hProcess, INFINITE);
	CloseHandle(hWrite);

	char buffer[4096];
	DWORD bytesRead;
	while (ReadFile(hRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
		buffer[bytesRead] = 0;
		output += buffer;
	}

	CloseHandle(hRead);

	DWORD exitCode;
	if (!GetExitCodeProcess(processInfo.hProcess, &exitCode)) {
		ren::throwException<std::runtime_error>("%s(): Failed to retrieve exit code; Error code: &d", __FUNCTION__, GetLastError());
	}

	std::pair<int, std::string> result = { exitCode, std::move(output) };

	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);

	return result;
}
