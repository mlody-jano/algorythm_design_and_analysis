cmake_minimum_required(VERSION 3.10.0)
project(Project_1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(SOURCES 
        src/main.cpp
        src/Ranking.cpp
        src/DynamicTable.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src)