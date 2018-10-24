cmake_minimum_required(VERSION 3.10)
project(pmaoa)

set(CMAKE_CXX_STANDARD 11)

include_directories(src lib/lemon/include)

add_executable(aco src/graph.cpp src/graph.h src/gnuplot.h src/bp_heuristic.h src/bp_heuristic.cpp src/aceHeuristics.h src/aceHeuristics.cpp)

add_executable(cw
        src/graph.cpp
        src/graph.h src/gnuplot.h src/bp_heuristic.h src/bp_heuristic.cpp
        src/draw.h src/draw.cpp src/bph_main.cpp src/gnuplot.cpp src/cw_heuristic.h src/cw_heuristic.cpp)
