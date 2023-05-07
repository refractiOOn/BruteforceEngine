#include "BruteforceEngine/BruteforceEngine.hpp"
#include "BruteforceEngine/ConfigurationHandler.hpp"
#include <iostream>

int main(int, char**)
{
    try
    {
        std::filesystem::path config = "C:/Users/Refraction_/Desktop/IT/Bruteforce/configuration.json";
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