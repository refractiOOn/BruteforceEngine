cmake_minimum_required(VERSION 3.26)
project(Bruteforce)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)