cmake_minimum_required(VERSION 3.14)

project(Example1)

set(CMAKE_CXX_STANDARD 14)
set(EXECUTABLE_OUTPUT_PATH ${Example1_SOURCE_DIR})

# include dirs
list (APPEND WSJCPP_INCLUDE_DIRS "src")
list (APPEND WSJCPP_INCLUDE_DIRS "../../src")

# Sources
list (APPEND WSJCPP_SOURCES "src/main.cpp")
list (APPEND WSJCPP_SOURCES "../../src/SimpleNeuralNetwork.cpp")


include_directories(${WSJCPP_INCLUDE_DIRS})

add_executable (Example1 ${WSJCPP_SOURCES})
 