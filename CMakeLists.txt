cmake_minimum_required(VERSION 3.16)
project(TheAssistantProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)
find_package(SQLite3 REQUIRED)

add_executable(TheAssistantProject
    src/main.cpp
    src/avatar_window.cpp
    src/avatar_window.h
    src/memory.cpp
    src/memory.h
    src/llm_engine.cpp
    src/llm_engine.h
)

target_link_libraries(TheAssistantProject PRIVATE 
    Qt6::Widgets 
    Qt6::Multimedia 
    SQLite3::SQLite3
)
