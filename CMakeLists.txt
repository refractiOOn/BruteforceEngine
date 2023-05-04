cmake_minimum_required(VERSION 3.26)
project(Bruteforce)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BruteforceEngine_HEADERS_DIR ${PROJECT_SOURCE_DIR}/src/BruteforceEngine)

include_directories(${BruteforceEngine_HEADERS_DIR})

add_subdirectory(src)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)