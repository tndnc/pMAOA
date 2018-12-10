cmake_minimum_required(VERSION 3.10)
project(pmaoa)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(Qt5_DIR "/Applications/Qt/5.12.0/clang_64/lib/cmake/Qt5")

# Find libraries
find_package(CPLEX)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(LEMON)

# Set source files
set(DRAW_SRC src/gnuplot.h src/gnuplot.cpp src/draw.h src/draw.cpp)
set(GRAPH_SRC src/graph.h src/graph.cpp)
set(IDESC_SRC src/iterative_descent.h src/iterative_descent.cpp)

# ----------------------------------------------------------------------------
# ACO heuristic
#
add_executable(aco ${GRAPH_SRC} ${DRAW_SRC} ${IDESC_SRC}
        src/aco_heuristic.h src/aco_heuristic.cpp src/aco_main.cpp)
target_link_libraries(aco lemon-library)
# --
add_executable(aco_test src/tests/aco_test.cpp
        ${GRAPH_SRC} src/aco_heuristic.cpp src/aco_heuristic.h ${IDESC_SRC})
target_link_libraries(aco_test lemon-library)

# ----------------------------------------------------------------------------
# Clark & Wright heuristic
#
add_executable(cw ${GRAPH_SRC} ${DRAW_SRC} ${IDESC_SRC}
        src/cw_heuristic.h src/cw_heuristic.cpp src/cw_main.cpp)
target_link_libraries(cw lemon-library)
# --
add_executable(cw_test ${GRAPH_SRC} ${IDESC_SRC}
        src/tests/cw_test.cpp src/cw_heuristic.cpp src/cw_heuristic.h)
target_link_libraries(cw_test lemon-library)

# ----------------------------------------------------------------------------
# BP heuristic
#
add_executable(bp ${DRAW_SRC} ${GRAPH_SRC} ${IDESC_SRC}
        src/bp_heuristic.h src/bp_heuristic.cpp src/bp_main.cpp)
target_link_libraries(bp lemon-library)
# --

# ----------------------------------------------------------------------------
# CPLEX VRP Solve
#
add_executable(cplex_solve src/cplex_solve.cpp ${GRAPH_SRC})
target_link_libraries(cplex_solve cplex-library cplex-concert ilocplex
        pthread m lemon-library)
target_compile_options(cplex_solve PRIVATE -m64 -O -fPIC -fno-strict-aliasing
        -fexceptions)