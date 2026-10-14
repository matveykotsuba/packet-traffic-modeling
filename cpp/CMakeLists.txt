cmake_minimum_required(VERSION 3.16)

project(traffic_model LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(traffic_model
    src/main.cpp
    src/EqualTrafficModel.cpp
    src/PoissonTrafficModel.cpp
    src/InputParser.cpp
    src/CsvWriter.cpp
)

target_include_directories(traffic_model PRIVATE include)

set_target_properties(traffic_model PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

target_compile_options(traffic_model PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
