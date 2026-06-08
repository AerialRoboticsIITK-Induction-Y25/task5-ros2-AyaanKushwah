# ── STAGE 1: BUILDER ─────────────────────────────────────────
FROM osrf/ros:humble-desktop AS builder

# install dependencies
RUN apt-get update && apt-get install -y \
    python3-colcon-common-extensions \
    python3-rosdep \
    && rm -rf /var/lib/apt/lists/*

# create workspace
WORKDIR /fleet_ws

# copy entire ROS2 workspace
COPY . /fleet_ws/

# initialize rosdep and install dependencies
RUN rosdep update && \
    rosdep install --from-paths src --ignore-src -r -y

# build workspace
RUN /bin/bash -c "source /opt/ros/humble/setup.bash && \
    colcon build --cmake-args -DCMAKE_BUILD_TYPE=Release"

# ── STAGE 2: RUNTIME ─────────────────────────────────────────
FROM osrf/ros:humble-desktop AS runtime

# install runtime dependencies
RUN apt-get update && apt-get install -y \
    python3-colcon-common-extensions \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /fleet_ws

# copy ONLY install directory from builder
COPY --from=builder /fleet_ws/install /fleet_ws/install

# write entrypoint inline — no separate file needed
RUN echo '#!/bin/bash\n\
set -e\n\
source /opt/ros/humble/setup.bash\n\
source /fleet_ws/install/setup.bash\n\
exec "$@"' > /ros_entrypoint.sh && \
    chmod +x /ros_entrypoint.sh

ENTRYPOINT ["/ros_entrypoint.sh"]
CMD ["ros2", "launch", "drone_fleet", "fleet.launch.py"]