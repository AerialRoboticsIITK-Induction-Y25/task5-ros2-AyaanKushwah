# ── STAGE 1: BUILDER ─────────────────────────────────────────
FROM osrf/ros:humble-desktop AS builder

RUN apt-get update && apt-get install -y \
    python3-colcon-common-extensions \
    python3-rosdep \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /fleet_ws

COPY . /fleet_ws/

RUN rosdep update && \
    rosdep install --from-paths src --ignore-src -r -y

RUN /bin/bash -c "source /opt/ros/humble/setup.bash && \
    colcon build --cmake-args -DCMAKE_BUILD_TYPE=Release"

# ── STAGE 2: RUNTIME ─────────────────────────────────────────
FROM osrf/ros:humble-desktop AS runtime

RUN apt-get update && apt-get install -y \
    python3-colcon-common-extensions \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /fleet_ws

COPY --from=builder /fleet_ws/install /fleet_ws/install

RUN echo '#!/bin/bash\nset -e\nsource /opt/ros/humble/setup.bash\nsource /fleet_ws/install/setup.bash\nexec "$@"' \
    > /ros_entrypoint.sh && chmod +x /ros_entrypoint.sh

ENTRYPOINT ["/ros_entrypoint.sh"]
CMD ["ros2", "launch", "drone_fleet", "fleet.launch.py"]