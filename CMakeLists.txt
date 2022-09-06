cmake_minimum_required(VERSION 3.0)

project(SimpleNeuralNetworkCpp)

set(CMAKE_CXX_STANDARD 14)
set(EXECUTABLE_OUTPUT_PATH ${SimpleNeuralNetworkCpp_SOURCE_DIR})

# Sources
list (APPEND WSJCPP_SOURCES "src/main.cpp")
list (APPEND WSJCPP_SOURCES "src/SimpleNeuralNetwork.cpp")


include_directories(${WSJCPP_INCLUDE_DIRS})

add_executable (SimpleNeuralNetworkCpp ${WSJCPP_SOURCES})
