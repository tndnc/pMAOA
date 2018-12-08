cmake_minimum_required(VERSION 3.10)
project(pmaoa)

set(CMAKE_CXX_STANDARD 11)

include_directories(src lib/lemon/include)

add_executable(aco
        src/graph.cpp src/graph.h
        src/gnuplot.h src/bp_heuristic.h src/bp_heuristic.cpp src/iterative_descent.h src/iterative_descent.cpp
        src/ace_heuristic.h src/ace_heuristic.cpp src/ace_main.cpp src/draw.h src/draw.cpp src/gnuplot.cpp)

add_executable(cw
        src/graph.cpp
        src/graph.h src/gnuplot.h
        src/draw.h src/draw.cpp src/cw_main.cpp src/gnuplot.cpp src/cw_heuristic.h src/cw_heuristic.cpp src/iterative_descent.h src/iterative_descent.cpp)

add_executable(cw_test src/tests/cw_test.cpp src/graph.h src/graph.cpp src/cw_heuristic.cpp src/cw_heuristic.h)

add_executable(bp
        src/graph.cpp
        src/graph.h src/gnuplot.h src/bp_heuristic.h src/bp_heuristic.cpp
        src/draw.h src/draw.cpp src/gnuplot.cpp src/bp_main.cpp src/iterative_descent.h src/iterative_descent.cpp)