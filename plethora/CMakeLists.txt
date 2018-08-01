cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# set to base install of jsoncpp-master
set(JSON_PARSER "/home/singer/misc/json/jsoncpp-master")

add_executable(main main.cpp)
add_library(Geometry Vertex.cpp Edge.cpp)

include_directories(${JSON_PARSER}/include)
link_directories(${JSON_PARSER}/build/debug/lib)

target_link_libraries(main PUBLIC Geometry PRIVATE ${JSON_PARSER}/build/debug/lib/libjsoncpp.a)
