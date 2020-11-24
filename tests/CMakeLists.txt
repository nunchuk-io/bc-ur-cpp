cmake_minimum_required(VERSION 3.1)
project(test-ur)

enable_testing()
add_library(unittest_main OBJECT src/unit.cpp)

set(files
    src/bc32_test.cpp
    src/ur_test.cpp
)

foreach(file ${files})
    get_filename_component(testcase ${file} NAME_WE)
    add_executable(${testcase} ${file} $<TARGET_OBJECTS:unittest_main>)
    target_link_libraries(${testcase} ur)
    target_include_directories(${testcase} PUBLIC "${PROJECT_SOURCE_DIR}/src")
    add_test(NAME ${testcase} COMMAND ${testcase})
endforeach()