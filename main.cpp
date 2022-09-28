#include "BruteforceEngine.h"

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
	bool configIsUsed = false;
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
	if (commandLineArgs.size() > 3)
	{
		if (commandLineArgs[3].compare("--use_config") == 0)
		{
			configIsUsed = true;
		}
		else
		{
			std::cout << "Invalid command-line args" << std::endl;
			return 0;
		}
	}

	BruteforceEngine engine(encryptedFile, plainFile);
	if (configIsUsed)
	{
		engine.SetConfig("config.txt");
	}
	if (passwordLogIsUsed)
	{
		engine.EnablePasswordLog("passwords_log.txt");
	}
	engine.Start();
}