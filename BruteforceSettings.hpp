#pragma once
#include "ISettings.hpp"
#include <filesystem>
#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>

class BruteforceSettings : public ISettings
{
public:
	BruteforceSettings(std::filesystem::path configFilePath);
	virtual ~BruteforceSettings() = default;

	virtual void Load() override;

	virtual void SetFilePath(std::filesystem::path configFilePath) override;
	void SetAmountOfCPUCores();

	virtual std::filesystem::path GetFilePath() const override;
	int GetAmountOfCPUCores() const;
	int GetMaxPasswordLength() const;
	const std::vector<char> &GetAdmissibleSymbols() const;
private:
	int m_amountOfCPUCores = 0;
	int m_maxPasswordLength = 0;
	std::vector<char> m_admissibleSymbols;

	std::filesystem::path m_configFilePath;
};