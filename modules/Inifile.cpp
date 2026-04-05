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
#include <ren/Stringtools.hpp>
#include <filesystem>

namespace fs = std::filesystem;

Inifile::Inifile() :
	p_lines(),
	p_apps(),
	p_filled(false) {}

Inifile::Inifile(std::istream& _source) :
	p_lines(),
	p_apps(),
	p_filled(false)
{
	fill(_source);
}


bool Inifile::fill(std::istream& _source)
{
	if (p_filled) {
		return false;
	}

	while (_source.good()) {
		std::string buffer;
		std::getline(_source, buffer);
		p_lines.push_back(std::move(buffer));
	}

	std::string currentApp;

	for (int i = 0; i < p_lines.size(); i++) {
		std::pair<std::string, std::string> dataAndComment = ren::st::splitKeyValuePair(p_lines[i], ';', "\t ");
		std::string& wheat = dataAndComment.first;
		if (wheat.size() == 0) {
			continue;
		}
		if (wheat[0] == '[') {
			size_t otherBracket = wheat.find(']');
			if (otherBracket == wheat.npos) {
				continue;
			}
			currentApp = ren::st::toUpper(std::string(wheat, 1, otherBracket - 1));
			continue;
		}

		std::pair<std::string, std::string> keyValuePair = ren::st::splitKeyValuePair(wheat, '=', "\t ");
		p_apps[currentApp].insert(std::pair(ren::st::toUpper(keyValuePair.first), i));
	}

	p_filled = true;
	return true;
}

std::string Inifile::getValueString(const std::string& _appName, const std::string& _keyName) const
{
	std::string appNameUpper = ren::st::toUpper(_appName);
	std::string keyNameUpper = ren::st::toUpper(_keyName);

	std::map<std::string, Linemap>::const_iterator itApp = p_apps.find(appNameUpper);
	if (itApp == p_apps.end()) {
		return std::string();
	}

	const Linemap& currentLinemap = itApp->second;
	Linemap::const_iterator itKey = currentLinemap.find(keyNameUpper);
	if (itKey == currentLinemap.end()) {
		return std::string();
	}

	std::pair<std::string, std::string> dataAndComment = ren::st::splitKeyValuePair(p_lines[itKey->second], ';', "\t ");
	std::pair<std::string, std::string> keyValuePair = ren::st::splitKeyValuePair(dataAndComment.first, '=', "\t ");
	return keyValuePair.second;
}

bool Inifile::setValueString(const std::string& _appName, const std::string& _keyName, const std::string& _value)
{
	std::string appNameUpper = ren::st::toUpper(_appName);
	std::string keyNameUpper = ren::st::toUpper(_keyName);

	std::map<std::string, Linemap>::iterator itApp = p_apps.find(appNameUpper);
	if (itApp == p_apps.end()) {
		return false;
	}
	
	Linemap& currentLinemap = itApp->second;
	Linemap::iterator itKey = currentLinemap.find(keyNameUpper);
	if (itKey == currentLinemap.end()) {
		return false;
	}

	std::pair<std::string, std::string> dataAndComment = ren::st::splitKeyValuePair(p_lines[itKey->second], ';', "\t ");
	std::pair<std::string, std::string> keyValuePair = ren::st::splitKeyValuePair(dataAndComment.first, '=', "\t ");

	if (dataAndComment.second.empty()) {
		p_lines[itKey->second] = keyValuePair.first + '=' + _value;
	}
	else {
		p_lines[itKey->second] = keyValuePair.first + '=' + _value + " ; " + dataAndComment.second;
	}	
	return true;
}

std::vector<std::string> Inifile::listApps() const
{
	std::vector<std::string> output;
	
	for (auto it = p_apps.begin(); it != p_apps.end(); it++) {
		output.push_back(it->first);
	}

	return output;
}

std::vector<std::string> Inifile::listKeys(const std::string& _appName) const
{
	std::vector<std::string> output;

	std::string appNameUpper = ren::st::toUpper(_appName);
	auto itApps = p_apps.find(appNameUpper);
	if (itApps == p_apps.end()) {
		return output;
	}

	const Linemap& currentApp = itApps->second;
	for (auto itKeys = currentApp.begin(); itKeys != currentApp.end(); itKeys++) {
		output.push_back(itKeys->first);
	}
	return output;
}

void Inifile::commit(std::ostream& _destination)
{
	for (int i = 0; i < p_lines.size() - 1; i++) {
		_destination << p_lines[i] << std::endl;
	}
	_destination << p_lines.back();
}

IniSession::IniSession(std::istream& _source, const std::string& _filename, bool _autocommit) : 
	inifile(_source),
	filename(_filename),
	autocommit(_autocommit) {}

InifileManager::InifileManager(bool _caseInsensitive) :
	p_iniSessions(),
	p_caseInsensitive(_caseInsensitive) {}

InifileManager::~InifileManager()
{
	for (auto it = p_iniSessions.begin(); it != p_iniSessions.end(); it++) {
		if (it->second.autocommit) {
			p_commit(&it->second);
		}
	}
}

IniSession* InifileManager::openIniSession(const std::string& _filename, bool _autocommit)
{
	IniSession* findResult = p_find(_filename);
	if (findResult) {
		return findResult;
	}
	else {
		return p_createSession(_filename, _autocommit);
	}
}

void InifileManager::closeIniSession(const std::string& _filename)
{
	p_close(_filename);
}

void InifileManager::closeIniSession(IniSession* _session)
{
	p_close(_session);
}

std::vector<IniSession*> InifileManager::getOpen()
{
	std::vector<IniSession*> result(p_iniSessions.size());
	auto it = p_iniSessions.begin();
	size_t i = 0;
	while (it != p_iniSessions.end() && i < result.size()) {
		result[i] = &it->second;
		it++;
		i++;
	}
	return result;
}

IniSession* InifileManager::getFirst()
{
	if (p_iniSessions.size()) {
		return &p_iniSessions.begin()->second;
	}
	else {
		return nullptr;
	}
}

bool InifileManager::renameSession(const std::string& _oldName, const std::string& _newName)
{
	return p_renameSession(_oldName, _newName);
}

bool InifileManager::renameSession(IniSession* _session, const std::string& _newName)
{
	return p_renameSession(_session, _newName);
}

bool InifileManager::commit(const std::string& _filename)
{
	IniSession* findResult = p_find(_filename);
	if (findResult) {
		return p_commit(findResult);
	}
	else {
		return false;
	}
}

bool InifileManager::commit(IniSession* _session)
{
	if (_session == nullptr) {
		return false;
	}
	for (auto it = p_iniSessions.begin(); it != p_iniSessions.end(); it++) {
		if (&it->second == _session) {
			return p_commit(_session);
		}
	}
	return false;
}


IniSession* InifileManager::p_createSession(const std::string& _filename, bool _autocommit)
{
	std::ifstream inputFile(_filename);
	if (!inputFile.is_open()) {
		return nullptr;
	}
	auto result = p_iniSessions.insert({ p_implementCiPolicy(_filename), IniSession(inputFile, _filename, _autocommit) });
	return &result.first->second;
}

IniSession* InifileManager::p_find(const std::string& _filename)
{
	auto it = p_iniSessions.find(p_implementCiPolicy(_filename));
	if (it == p_iniSessions.end()) {
		return nullptr;
	}
	else {
		return &it->second;
	}
}

bool InifileManager::p_close(const std::string& _filename)
{
	auto it = p_iniSessions.find(p_implementCiPolicy(_filename));
	if (it == p_iniSessions.end()) {
		return true;
	}
	if (it->second.autocommit) {
		if (p_commit(&it->second)) {
			p_iniSessions.erase(it);
			return true;
		}
		else {
			return false;
		}
	}
	else {
		p_iniSessions.erase(it);
		return true;
	}
}

bool InifileManager::p_close(IniSession* _session)
{
	for (auto it = p_iniSessions.begin(); it != p_iniSessions.end(); it++) {
		if (&it->second == _session) {
			if (it->second.autocommit) {
				if (p_commit(&it->second)) {
					p_iniSessions.erase(it);
					return true;
				}
				else {
					return false;
				}
			}
			else {
				p_iniSessions.erase(it);
				return true;
			}
		}
	}
	return true;
}

bool InifileManager::p_renameSession(const std::string& _oldName, const std::string& _newName)
{
	std::string oldNamePolicy = p_implementCiPolicy(_oldName);
	std::string newNamePolicy = p_implementCiPolicy(_newName);
	if (oldNamePolicy == newNamePolicy) {
		return true;
	}
	auto itNew = p_iniSessions.find(newNamePolicy);
	if (itNew != p_iniSessions.end()) {
		return false;
	}

	auto itOld = p_iniSessions.find(oldNamePolicy);
	if (itOld == p_iniSessions.end()) {
		return false;
	}	

	IniSession tempSession = std::move(itOld->second);
	p_iniSessions.erase(itOld);
	tempSession.filename = _newName;
	p_iniSessions.insert({ newNamePolicy, std::move(tempSession) });
	return true;
}

bool InifileManager::p_renameSession(IniSession* _session, const std::string& _newName)
{
	std::string newNamePolicy = p_implementCiPolicy(_newName);
	auto itNew = p_iniSessions.find(newNamePolicy);
	if (itNew != p_iniSessions.end()) {
		return false;
	}

	for (auto itOld = p_iniSessions.begin(); itOld != p_iniSessions.end(); itOld++) {
		if (&itOld->second == _session) {
			IniSession tempSession = std::move(itOld->second);
			p_iniSessions.erase(itOld);
			tempSession.filename = _newName;
			p_iniSessions.insert({ p_implementCiPolicy(_newName), std::move(tempSession) });
			return true;
		}
	}
	return false;
}

bool InifileManager::p_commit(IniSession* _session)
{
	std::string tempFilename = _session->filename + ".temp";
	std::ofstream fileOutput(tempFilename, std::ios::trunc);
	if (!fileOutput.is_open()) {
		return false;
	}
	_session->inifile.commit(fileOutput);
	fileOutput.close();
	try {
		fs::rename(fs::path(tempFilename), fs::path(_session->filename));
		return true;
	}
	catch (...) {
		return false;
	}
}

inline std::string InifileManager::p_implementCiPolicy(const std::string& _string)
{
	return (p_caseInsensitive) ? ren::st::toUpper(_string) : _string;
}



