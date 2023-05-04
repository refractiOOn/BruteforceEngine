set(SOURCE_FILES main.cpp)

add_subdirectory(BruteforceEngine)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC BruteforceEngine)