cmake_minimum_required(VERSION 3.10)
project(HyperHeuristicDemo CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(hh_demo
    src/main.cpp
    src/BinPackingDomain.cpp
    src/HyperHeuristic.cpp
)