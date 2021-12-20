# CMakeList.txt: проект CMake для expression_parser; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

# Добавьте источник в исполняемый файл этого проекта.
add_executable (expression_parser "expression_parser.cpp" "expression_parser.h")

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
