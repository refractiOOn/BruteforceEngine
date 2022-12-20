#include "BruteforceSettings.hpp"
#include "BruteforceEngine.h"
#include <iostream>
#include <memory>
#include <filesystem>
#include <string>

int main(size_t argc, char* argv[])
{
	std::vector<std::string> commandLineArgs;
	for (size_t i = 1; i < argc; ++i)
	{
		commandLineArgs.push_back(argv[i]);
	}
	fs::path encryptedFile;
	fs::path plainFile;
	bool passwordLogIsUsed = false;
	if (commandLineArgs.size() < 2)
	{
		std::cout << "Invalid command-line args" << std::endl;
		return 0;
	}
	encryptedFile = commandLineArgs[0];
	plainFile = commandLineArgs[1];
	if (commandLineArgs.size() > 2)
	{
		if (commandLineArgs[2].compare("--log_passwords") == 0)
		{
			passwordLogIsUsed = true;
		}
		else
		{
			std::cout << "Invalid command-line args" << std::endl;
			return 0;
		}
	}

	std::filesystem::path configFile = "config.txt";
	BruteforceSettings settings(configFile);
	settings.Load();

	BruteforceEngine engine(encryptedFile, plainFile);
	engine.AddConfig(&settings);
	if (passwordLogIsUsed)
	{
		engine.EnablePasswordLog("passwords_log.txt");
	}
	engine.Start();
}