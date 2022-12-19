#include "Settings.hpp"
#include "magic_enum/magic_enum.hpp"
#include <fstream>
#include <string>
#include <algorithm>

Settings::Settings(std::filesystem::path configFilePath) : m_configFilePath(configFilePath)
{
}

Settings::~Settings()
{
}

void Settings::Load()
{
	std::fstream file(m_configFilePath);
	std::string temp;
	while (std::getline(file, temp, ';'))
	{
		// CONFIG FORMAT
		// key: value;
		// key: value;
		temp.erase(std::remove(temp.begin(), temp.end(), ' '), temp.end());
		temp.erase(std::remove(temp.begin(), temp.end(), '\n'), temp.end());

		std::string value(++std::find(temp.begin(), temp.end(), ':'), temp.end());

		std::string str(temp.begin(), std::find(temp.begin(), temp.end(), ':'));
		auto a = magic_enum::enum_cast<SettingName>(str);
		SettingName key;
		if (a.has_value())
		{
			key = a.value();
			m_settingCollection.emplace(key, value);
		}
	}
}

std::filesystem::path Settings::GetConfigFilePath() const
{
	return m_configFilePath;
}

void Settings::AddSetting(SettingName key, std::string_view value)
{
	m_settingCollection.emplace(key, value);
}

std::string Settings::operator[](SettingName settingName) const
{
	return (*m_settingCollection.find(settingName)).second;
}
