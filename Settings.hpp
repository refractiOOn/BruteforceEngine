#pragma once
#include "SettingName.hpp"
#include <filesystem>
#include <unordered_map>
#include <string>
#include <string_view>

class Settings
{
	typedef std::unordered_map<SettingName, std::string> SettingCollection;
public:
	Settings(std::filesystem::path configFilePath);
	~Settings();

	void Load();
	std::filesystem::path GetConfigFilePath() const;

	void AddSetting(SettingName key, std::string_view value);
	std::string operator[](SettingName settingName) const;
private:
	std::filesystem::path m_configFilePath;
	SettingCollection m_settingCollection;
};