cmake_minimum_required(VERSION 2.6)

project(allTests)

# message("prefix: ${CMAKE_FIND_LIBRARY_PREFIXES}")
# message("suffix: ${CMAKE_FIND_LIBRARY_SUFFIXES}")
message("PROJ_SOURCE_DIR: ${PROJECT_SOURCE_DIR}") 


# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
include_directories("${PROJECT_SOURCE_DIR}")
include_directories(../include ../external ./)

#add_subdirectory(../src/ ../obj)

set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11")

file(GLOB TESTS_SOURCES "src/*.cpp")
file(GLOB APP_SOURCES "../src/*.cpp")

set(SOURCES ${APP_SOURCES} ${TESTS_SOURCES})

# Link allTests with what we want to test and the GTest and pthread library
add_executable(allTests.o ${SOURCES})

target_link_libraries(allTests.o ${GTEST_LIBRARIES} pthread jpeg z m X11)
