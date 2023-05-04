cmake_minimum_required(VERSION 3.26)
project(Bruteforce)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BruteforceEngine_HEADERS_DIR ${PROJECT_SOURCE_DIR}/src/BruteforceEngine)

include(CTest)
enable_testing()

include_directories(${BruteforceEngine_HEADERS_DIR})

add_subdirectory(src)

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

target_link_libraries(${PROJECT_NAME}
    ${OpenSSL_LIBRARIES}
    ${RapidJSON_LIBRARIES}
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)