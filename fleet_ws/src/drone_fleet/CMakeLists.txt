cmake_minimum_required(VERSION 3.8)
project(drone_fleet)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)

# Point directly to your nested headers so you don't have to rewrite your .cpp includes
include_directories(include/drone_fleet)

include_directories(
    include
    ../../../part1/include       # sometimes needed too
)

# 1. Drone Node Executable
add_executable(drone_node 
  src/drone_node.cpp
  ../../../part1/src/vehicle.cpp
  ../../../part1/src/drone.cpp
  ../../../part1/src/autonomous_drone.cpp
  ../../../part1/src/mission_drone.cpp
  
)
ament_target_dependencies(drone_node rclcpp std_msgs)

# 2. Fleet Manager Executable
add_executable(fleet_manager src/fleet_manager.cpp)
ament_target_dependencies(fleet_manager rclcpp std_msgs std_srvs)

# 3. Health Monitor Executable
add_executable(health_monitor src/health_monitor.cpp)
ament_target_dependencies(health_monitor rclcpp std_msgs)

# Install targets
install(TARGETS
  drone_node
  fleet_manager
  health_monitor
  DESTINATION lib/${PROJECT_NAME}
)

# Optional: if you make a launch file later
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME}/)


ament_package()