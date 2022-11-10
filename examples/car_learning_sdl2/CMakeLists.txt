cmake_minimum_required(VERSION 3.14)

set(PROJECT_NAME example_car_learning_sdl2)

project(${PROJECT_NAME})

set(EXECUTABLE_OUTPUT_PATH ${${PROJECT_NAME}_SOURCE_DIR}/../../)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")

set(CMAKE_CXX_STANDARD 17)

add_executable(
    ${PROJECT_NAME} 
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
    "${PROJECT_SOURCE_DIR}/src/car_simulator.cpp"
    "${PROJECT_SOURCE_DIR}/../../src/SimpleNeuralNetwork.cpp"
)



if(NOT APPLE)
  find_package(Threads)
endif()

list (APPEND CAR_LEARNING_SDL2LIBRARIES ${CMAKE_THREAD_LIBS_INIT})

list (APPEND CAR_LEARNING_SDL2_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src")
list (APPEND CAR_LEARNING_SDL2_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/../../src")

############################
# SDL2
find_package(SDL2 REQUIRED)
if ( SDL2_FOUND )
    list (APPEND CAR_LEARNING_SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
    list (APPEND CAR_LEARNING_SDL2_LIBRARIES ${SDL2_LIBRARY})
endif( SDL2_FOUND )
############################

############################
# SDL2_image
find_package(SDL2_image REQUIRED)
if ( SDL2_image_FOUND )
    list (APPEND CAR_LEARNING_SDL2_INCLUDE_DIRS ${SDL2_IMAGE_INCLUDE_DIRS})
    list (APPEND CAR_LEARNING_SDL2_LIBRARIES ${SDL2_IMAGE_LIBRARIES})
    
endif( SDL2_image_FOUND )
############################

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${CAR_LEARNING_SDL2_INCLUDE_DIRS}
)
 
target_link_libraries(
    ${PROJECT_NAME}
    ${CAR_LEARNING_SDL2_LIBRARIES}
)