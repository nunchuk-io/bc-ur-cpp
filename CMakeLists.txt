cmake_minimum_required(VERSION 3.1)
project(ur VERSION 0.1.0)

option(UR__DISABLE_TESTS "If tests should be compiled or not" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library("${PROJECT_NAME}" STATIC 
    src/bc32.cpp 
    src/ur.cpp 
    src/utils.cpp
)

if(WIN32)
    target_compile_options("${PROJECT_NAME}" PUBLIC /MT)
    target_link_options("${PROJECT_NAME}" PUBLIC /INCREMENTAL:NO /NODEFAULTLIB:MSVCRT)
endif()

target_include_directories("${PROJECT_NAME}" PUBLIC 
    "${PROJECT_SOURCE_DIR}/src"
)

if (NOT UR__DISABLE_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
