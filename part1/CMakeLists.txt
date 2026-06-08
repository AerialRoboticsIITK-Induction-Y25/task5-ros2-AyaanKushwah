cmake_minimum_required(VERSION 3.10)
project(DroneFleet)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra)

add_executable(drone_fleet
    src/main.cpp
    src/vehicle.cpp
    src/drone.cpp
    src/mission_drone.cpp
    src/autonomous_drone.cpp
)

target_include_directories(drone_fleet PRIVATE
    part1/include
)