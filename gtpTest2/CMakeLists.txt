cmake_minimum_required(VERSION 3.5)

project(ue_ran)
message(STATUS "Compiling Network Function : ${PROJECT_NAME}")
set(CMAKE_CXX_STANDARD 11)

message(STATUS "Project Directory: ${PROJECT_SOURCE_DIR}")

# set a build diractory
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
#set a bin directory
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# add all necessary libs
add_subdirectory(../libgtp bin_libgtp)


# add all include paths
include_directories(${PROJECT_SOURCE_DIR})
include_directories(../libgtp/include)

set(BASIC_LIBRARIES "-lpthread")
add_executable(${PROJECT_NAME} client.cpp)
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -g -fsanitize=address")
target_link_libraries(${PROJECT_NAME} ${BASIC_LIBRARIES} libGTP)