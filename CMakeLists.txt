cmake_minimum_required (VERSION 3.20)

project (libido)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules")

find_package (CURL)
find_package (JsonC)

include_directories (include)

add_subdirectory (src)
add_subdirectory (examples)

