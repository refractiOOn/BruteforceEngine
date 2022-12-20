#pragma once

#include <filesystem>

class ISettings
{
public:
	virtual ~ISettings() = default;

	virtual void Load() = 0;
	virtual void SetFilePath(std::filesystem::path configFilePath) = 0;
	virtual std::filesystem::path GetFilePath() const = 0;
};