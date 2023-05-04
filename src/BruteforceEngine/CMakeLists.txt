cmake_minimum_required(VERSION 3.26)
project(BruteforceEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILES
    BruteforceEngine.hpp BruteforceEngine.cpp
    ConfigurationHandler.hpp ConfigurationHandler.cpp
    FileHandler.hpp FileHandler.cpp
    ProgressTracker.hpp ProgressTracker.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    message("OpenSSL was successfully found")
else()
    message(FATAL_ERROR "OpenSSL was not found")
endif()

find_package(RapidJSON REQUIRED)
if(RapidJSON_FOUND)
    message("RapidJSON was successfully found")
else()
    message(FATAL_ERROR "RapidJSON was not found")
endif()

find_package(WIL REQUIRED)
if(WIL_FOUND)
    message("WIL was successfully found")
else()
    message(FATAL_ERROR "WIL was not found")
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC
    ${OpenSSL_LIBRARIES}
    ${RapidJSON_LIBRARIES}
    ${wil_LIBRARIES}
)