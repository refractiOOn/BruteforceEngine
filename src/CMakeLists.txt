set(SRC_FILES main.cpp
    BruteforceEngine/BruteforceEngine.hpp BruteforceEngine/BruteforceEngine.cpp
    BruteforceEngine/ConfigurationHandler.hpp BruteforceEngine/ConfigurationHandler.cpp
    BruteforceEngine/FileHandler.hpp BruteforceEngine/FileHandler.cpp
    BruteforceEngine/Logger.hpp BruteforceEngine/Logger.cpp
    BruteforceEngine/ProgressTracker.hpp BruteforceEngine/ProgressTracker.cpp
    BruteforceEngine/cryptography.hpp BruteforceEngine/cryptography.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

#target_include_directories(${PROJECT_NAME} PUBLIC BruteforceEngine)