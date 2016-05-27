cmake_minimum_required(VERSION 2.8)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -g")
add_executable (monitor monitor.cpp ProcessInfoList.cpp ProcessInfo.cpp)
target_link_libraries (monitor boost_program_options)
