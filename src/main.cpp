#include "BruteforceEngine/BruteforceEngine.hpp"
#include "BruteforceEngine/ConfigurationHandler.hpp"
#include <iostream>

int main(int argc, char** argv)
{
    try
    {
        if (argc < 2)
        {
            throw std::runtime_error("No configuration file provided");
        }

        std::filesystem::path config = argv[1];
        bruteforce::ConfigurationHandler configHandler(config);
        std::shared_ptr<bruteforce::ConfigurationContainer> configContainer = configHandler.load();

        bruteforce::BruteforceEngine engine(configContainer);
        engine.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}