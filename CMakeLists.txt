cmake_minimum_required(VERSION 3.0)

set(PROJECT_NAME SimpleNeuralNetworkCpp)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)
set(EXECUTABLE_OUTPUT_PATH ${${PROJECT_NAME}_SOURCE_DIR})

include_directories(
    "${PROJECT_SOURCE_DIR}/src"
)

add_executable(
    ${PROJECT_NAME} 
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
    "${PROJECT_SOURCE_DIR}/src/SimpleNeuralNetwork.cpp"
)

# enable testing functionality
enable_testing()
add_subdirectory(tests)

# examples
add_subdirectory(examples/sum_numbers)
add_subdirectory(examples/car_learning)
add_subdirectory(examples/mesh_calc_tangents)
