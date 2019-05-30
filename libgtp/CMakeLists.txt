cmake_minimum_required(VERSION 3.5)

include_directories(include/)

file(GLOB GTP_SRC src/*.cpp)

add_library(libGTP STATIC ${GTP_SRC})