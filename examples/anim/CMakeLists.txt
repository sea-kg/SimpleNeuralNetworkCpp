cmake_minimum_required(VERSION 3.14)

set(PROJECT_NAME example_anim)

project(${PROJECT_NAME})
set(EXECUTABLE_OUTPUT_PATH ${${PROJECT_NAME}_SOURCE_DIR}/../../)

set(CMAKE_CXX_STANDARD 14)

add_executable(
    ${PROJECT_NAME} 
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
    "${PROJECT_SOURCE_DIR}/../../src/SimpleNeuralNetwork.cpp"
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/../../src"
)
 