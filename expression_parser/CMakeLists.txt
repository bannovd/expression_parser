cmake_minimum_required (VERSION 3.8)

project(expression_parser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable (expression_parser "main.cpp" "expression_parser.h" "parser.h" )

