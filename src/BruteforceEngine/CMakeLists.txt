cmake_minimum_required(VERSION 3.26)
project(BruteforceEngine)

set(SOURCE_FILES
    BruteforceEngine.hpp BruteforceEngine.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})