#!/bin/bash

docker run -it \
  --rm \
  --network=host \
  -e ROS_DOMAIN_ID=0 \
  -v "$(pwd)":/fleet_ws \
  drone_fleet \
  ros2 launch drone_fleet fleet.launch.py