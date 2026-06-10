cmake_minimum_required(VERSION 3.10)
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

# go up from fleet_ws/src/drone_fleet/ to repo root, then into part1/include
set(PART1_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../part1)

include_directories(
  ${PART1_ROOT}/include
)

# 1. Drone Node
add_executable(drone_node
  src/drone_node.cpp
  ${PART1_ROOT}/src/vehicle.cpp
  ${PART1_ROOT}/src/drone.cpp
  ${PART1_ROOT}/src/autonomous_drone.cpp
  ${PART1_ROOT}/src/mission_drone.cpp
)
ament_target_dependencies(drone_node rclcpp std_msgs)

# 2. Fleet Manager
add_executable(fleet_manager
  src/fleet_manager.cpp
  ${PART1_ROOT}/src/vehicle.cpp
  ${PART1_ROOT}/src/drone.cpp
  ${PART1_ROOT}/src/autonomous_drone.cpp
  ${PART1_ROOT}/src/mission_drone.cpp
)
ament_target_dependencies(fleet_manager rclcpp std_msgs std_srvs)

# 3. Health Monitor
add_executable(health_monitor
  src/health_monitor.cpp
  ${PART1_ROOT}/src/vehicle.cpp
  ${PART1_ROOT}/src/drone.cpp
  ${PART1_ROOT}/src/autonomous_drone.cpp
  ${PART1_ROOT}/src/mission_drone.cpp
)
ament_target_dependencies(health_monitor rclcpp std_msgs)

install(TARGETS
  drone_node
  fleet_manager
  health_monitor
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch DESTINATION share/${PROJECT_NAME}/)

ament_package()