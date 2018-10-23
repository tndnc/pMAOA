cmake_minimum_required(VERSION 3.10)
project(pmaoa)

set(CMAKE_CXX_STANDARD 11)

include_directories(src lib/lemon/include)

add_executable(pmaoa
        src/graph.cpp
        src/graph.h src/gnuplot.h src/heuristic_c.h src/heuristic_c.cpp src/aceHeuristics.h src/aceHeuristics.cpp)
