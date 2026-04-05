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
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <list>

class Inifile
{
public:
	Inifile();
	Inifile(std::istream& _source);
	Inifile(const Inifile& _other) = default;
	Inifile(Inifile&& _other) = default;
	~Inifile() = default;

	bool fill(std::istream& _source);

	std::string getValueString(const std::string& _appName, const std::string& _keyName) const;
	bool setValueString(const std::string& _appName, const std::string& _keyName, const std::string& _value);

	std::vector<std::string> listApps() const;
	std::vector<std::string> listKeys(const std::string& _appName) const;

	void commit(std::ostream& _destination);

private:
	typedef std::map<std::string, int> Linemap;

	std::vector<std::string> p_lines;
	std::map<std::string, Linemap> p_apps;

	bool p_filled;
};

struct IniSession
{
	IniSession() = default;
	IniSession(const IniSession& _other) = default;
	IniSession(IniSession&&) = default;
	IniSession(std::istream& _source, const std::string& _filename, bool _autocommit = true);

	Inifile inifile;
	std::string filename;
	bool autocommit;
};

class InifileManager
{
public:
	InifileManager() = delete;
	InifileManager(bool _caseInsensitive);
	InifileManager(InifileManager&&) = delete;
	InifileManager(const InifileManager&) = delete;
	~InifileManager();

	IniSession* openIniSession(const std::string& _filename, bool _autocommit);
	void closeIniSession(const std::string& _filename);
	void closeIniSession(IniSession* _session);

	std::vector<IniSession*> getOpen();
	IniSession* getFirst();

	bool renameSession(const std::string& _oldName, const std::string& _newName);
	bool renameSession(IniSession* _session, const std::string& _newName);

	bool commit(const std::string& _filename);
	bool commit(IniSession* _session);

private:
	IniSession* p_createSession(const std::string& _filename, bool _autocommit);
	IniSession* p_find(const std::string& _filename);
	bool p_close(const std::string& _filename);
	bool p_close(IniSession* _session);

	bool p_renameSession(const std::string& _oldName, const std::string& _newName);
	bool p_renameSession(IniSession* _session, const std::string& _newName);

	bool p_commit(IniSession* _session);
	std::string p_implementCiPolicy(const std::string& _string);

	std::map<std::string, IniSession> p_iniSessions;
	bool p_caseInsensitive;
};
