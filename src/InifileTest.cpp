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

#include "Inifile.h"
#include <fstream>
#include <iostream>
#include <ren/Stringtools.hpp>
#include <filesystem>
#include <utility>
#include <map>

typedef void(*CommandHandler)(const std::vector<std::string>& tokens);

// =============
//  Gobal state
// =============

// Inifile* g_inifile = nullptr;
// std::string g_currentFilename;

InifileManager g_inifileManager(true);
IniSession* g_currentSession = nullptr;

// ==================
//  Command handlers
// ==================

void handleCommandExit(const std::vector<std::string>& _tokens)
{
	exit(0);
}

void handleCommandList(const std::vector<std::string>& _tokens)
{
	constexpr char subcommandApps[] = "APPS";
	constexpr char subcommandApp[] = "APP";
	constexpr char subcommandOpen[] = "OPEN";

	if (_tokens.size() < 2) {
		std::cout << "Invalid usage. Usage: \n"
			"LIST OPEN\n"
			"LIST APPS\n"
			"LIST APP \"AppName\"" << std::endl;
		return;
	}

	std::string commandUpper = ren::st::toUpper(_tokens[1]);
	if (commandUpper == subcommandOpen) {
		std::vector<IniSession*> sessionList = g_inifileManager.getOpen();
		if (sessionList.size() == 0) {
			std::cout << "[No files opened.]" << std::endl;
			return;
		}
		for (size_t i = 0; i < sessionList.size(); i++) {
			std::cout << sessionList[i]->filename << std::endl;
		}
		return;
	}

	if (g_currentSession == nullptr) {
		std::cout << "No INI file loaded!" << std::endl;
		return;
	}	

	if (commandUpper == subcommandApp) {
		if (_tokens.size() < 3) {
			std::cout << "Invalid usage. Usage: \n"
				"LIST APP [AppName]" << std::endl;
			return;
		}
		std::vector<std::string> output = g_currentSession->inifile.listKeys(_tokens[2]);
		for (int i = 0; i < output.size(); i++) {
			std::cout << output[i] << std::endl;
		}
		return;
	}
	if (commandUpper == subcommandApps) {
		std::vector<std::string> output = g_currentSession->inifile.listApps();
		for (int i = 0; i < output.size(); i++) {
			std::cout << output[i] << std::endl;
		}
		return;
	}

	std::cout << '\"' << _tokens[1] << "\" is not a valid command." << std::endl;
}

void handleCommandOpen(const std::vector<std::string>& _tokens)
{
	if (_tokens.size() < 2) {
		std::cout << "Invalid usage. Usage: "
			"OPEN [filename]" << std::endl;
		return;
	}

	IniSession* newSession = g_inifileManager.openIniSession(_tokens[1], false);
	if (newSession == nullptr) {
		std::cout << '\"' << _tokens[1] << "\" could not be opened. (Does it exist?)" << std::endl;
		return;
	}
	g_currentSession = newSession;
	std::cout << '\"' << g_currentSession->filename << "\" was opened!" << std::endl;

	return;
}

void handleCommandGet(const std::vector<std::string>& _tokens)
{
	if (g_currentSession == nullptr) {
		std::cout << "No INI file selected!" << std::endl;
		return;
	}
	if (_tokens.size() < 3) {
		std::cout << "Invalid usage. Usage: "
			"GET [App Name] [Key Name]" << std::endl;
		return;
	}
	std::string result = g_currentSession->inifile.getValueString(_tokens[1], _tokens[2]);
	std::cout << ">>> " << result << std::endl;
	return;
}

void handleCommandSet(const std::vector<std::string>& _tokens)
{
	if (g_currentSession == nullptr) {
		std::cout << "No INI file selected!" << std::endl;
		return;
	}
	if (_tokens.size() < 4) {
		std::cout << "Invalid usage. Usage: "
			"SET [App Name] [Key Name] [Value]" << std::endl;
		return;
	}
	if (g_currentSession->inifile.setValueString(_tokens[1], _tokens[2], _tokens[3])) {
		std::cout << "Value set!" << std::endl;
	}
	else {
		std::cout << "Could not set value." << std::endl;
	}
	return;
}

void handleCommandClear(const std::vector<std::string>& _tokens)
{
	if (g_currentSession == nullptr) {
		std::cout << "No INI file selected!" << std::endl;
		return;
	}
	if (_tokens.size() < 3) {
		std::cout << "Invalid usage. Usage: "
			"CLEAR [App Name] [Key Name]" << std::endl;
		return;
	}
	if (g_currentSession->inifile.setValueString(_tokens[1], _tokens[2], std::string())) {
		std::cout << "Value cleared!" << std::endl;
	}
	else {
		std::cout << "Could not clear value." << std::endl;
	}
	return;
}

void handleCommandCommit(const std::vector<std::string>& _tokens)
{
	if (g_currentSession == nullptr) {
		std::cout << "No INI file selected!" << std::endl;
		return;
	}

	if (g_inifileManager.commit(g_currentSession)) {
		std::cout << '\"' << g_currentSession->filename << "\" written!" << std::endl;
		return;
	}
	else {
		std::cout << "Could not write \"" << g_currentSession->filename << "\"." << std::endl;
		return;
	}
}

void handleCommandSaveAs(const std::vector<std::string>& _tokens)
{
	if (g_currentSession == nullptr) {
		std::cout << "No INI file selected!" << std::endl;
		return;
	}

	if (_tokens.size() < 2) {
		std::cout << "Invalid usage. Usage: "
			"SAVEAS [Filename]." << std::endl;
		return;
	}

	if (!g_inifileManager.renameSession(g_currentSession, _tokens[1])) {
		std::cout << "Could not save as \"" << _tokens[1] << "\"." << std::endl;
		return;
	}
	g_currentSession = g_inifileManager.openIniSession(_tokens[1], false);

	if (!g_inifileManager.commit(g_currentSession)) {
		std::cout << "Could not write to \"" << _tokens[1] << "\"." << std::endl;
		return;
	}

	std::cout << '\"' << g_currentSession->filename << "\" written!" << std::endl;
	return;
}

void handleCommandCurrent(const std::vector<std::string>& _tokens)
{
	if (g_currentSession) {
		std::cout << ">>> " << g_currentSession->filename << std::endl;
	}
	else {
		std::cout << "No file selected." << std::endl;
	}
	return;
}

void handleCommandClose(const std::vector<std::string>& _tokens)
{
	if (_tokens.size() < 2) {
		if (g_currentSession == nullptr) {
			std::cout << "No file selected to close." << std::endl;
			return;
		}
		std::string oldFilename = g_currentSession->filename;
		g_inifileManager.closeIniSession(g_currentSession);
		std::cout << "Closed \"" << oldFilename << "\". ";
		g_currentSession = g_inifileManager.getFirst();
		if (g_currentSession) {
			std::cout << "Active file: \"" << g_currentSession->filename << "\"." << std::endl;
			return;
		}
		else {
			std::cout << "No files active." << std::endl;
			return;
		}
	}
	else {
		std::cout << "This command takes no argument." << std::endl;
		return;
	}
}

// =============
//  Command Map
// =============

constexpr char commandExit[] = "EXIT";
constexpr char commandList[] = "LIST";
constexpr char commandOpen[] = "OPEN";
constexpr char commandGet[] = "GET";
constexpr char commandSet[] = "SET";
constexpr char commandClear[] = "CLEAR";
constexpr char commandCommit[] = "COMMIT";
constexpr char commandSaveAs[] = "SAVEAS";
constexpr char commandCurrent[] = "CURRENT";
constexpr char commandClose[] = "CLOSE";

std::map<std::string, CommandHandler> g_commandMap = {
	{commandExit, handleCommandExit},
	{commandList, handleCommandList},
	{commandOpen, handleCommandOpen},
	{commandGet, handleCommandGet},
	{commandSet, handleCommandSet},
	{commandClear, handleCommandClear},
	{commandCommit, handleCommandCommit},
	{commandSaveAs, handleCommandSaveAs},
	{commandCurrent, handleCommandCurrent},
	{commandClose, handleCommandClose}
};

// ===========
//  Main loop
// ===========

int main(int _argc, char** _argv)
{
	while (true) {
		std::cout << "InifileTest> ";
		std::string lineInput;
		std::getline(std::cin, lineInput);
		auto tokens = ren::st::dissect(lineInput, "\t ", "\"\"");
		if (tokens.size() == 0) {
			continue;
		}
		std::string commandUpper = ren::st::toUpper(tokens[0]);
		auto it = g_commandMap.find(commandUpper);
		if (it == g_commandMap.end()) {
			std::cout << '\"' << commandUpper << "\" is not a valid command." << std::endl;
		}
		else {
			it->second(tokens);
		}
	}
}