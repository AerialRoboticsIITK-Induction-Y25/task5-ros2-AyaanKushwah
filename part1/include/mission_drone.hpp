#pragma once
#include "drone.hpp"
#include <string>
#include <vector>
#include <tuple>
// #include "timestamp.h"

class MissionDrone : public Drone{

private:
    std::vector<std::pair<std::tuple<float,float,float>, std::string>> visited_waypoints;

protected:
    std::vector<std::tuple<float, float, float>> waypoints;
    int current_waypoint_index;
    std::string mission_name;
    

public:

    MissionDrone();
    std::tuple<float, float,float> next_waypoint();
    
    void skip_waypoint(const std::string& reason);

    bool mission_complete();

    MissionDrone(const std::string& drone_name, const float& battery_level, const float& max_altitude, const std::string& mission_name,  const std::vector<std::tuple<float, float, float>>& waypoints); //Constructor    
    ~MissionDrone(); //Destructor

    std::string mission_summary();

    std::string get_info() override;

    std::string get_mission_name();
    
    std::string get_visited_waypoints();

    std::string get_waypoints();
    
    float get_altitude();

    int get_current_waypoint_index();

    void set_mission_name(const std::string& mission_name);

};