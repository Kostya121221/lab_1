cmake_minimum_required(VERSION 3.16)
project(lab1_avl LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(nlohmann_json 3.2.0 REQUIRED)
add_executable(dbms
    src/main.cpp
    src/array.cpp
    src/singly_list.cpp
    src/doubly_list.cpp
    src/stack.cpp
    src/queue.cpp
    src/avl_tree.cpp
    src/storage.cpp
)
target_link_libraries(dbms PRIVATE nlohmann_json::nlohmann_json)
target_compile_options(dbms PRIVATE -Wall -Wextra -Wpedantic)
