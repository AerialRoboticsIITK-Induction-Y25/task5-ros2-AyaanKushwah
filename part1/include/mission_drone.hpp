#pragma once
#include "drone.hpp"
#include <string>
#include <vector>
#include <tuple>
// #include "timestamp.h"

class MissionDrone : public Drone{

private:
    
    std::string mission_name;
    std::vector<std::pair<std::tuple<float,float,float>, std::string>> visited_waypoints;

protected:
    std::vector<std::tuple<float, float, float>> waypoints;
    int current_waypoint_index;
    

public:

    MissionDrone();
    std::tuple<float, float,float> next_waypoint();
    
    void skip_waypoint(const std::string& reason);

    bool mission_complete();

    MissionDrone(const std::string& drone_name, const float& battery_level, const float& max_altitude, const std::string& mission_name,  const std::vector<std::tuple<float, float, float>>& waypoints); //Constructor    
    ~MissionDrone(); //Destructor

    std::string mission_summary();

    void get_info() override;

    std::string getMissionName();
    
    std::string getVisitedWaypoints();

    std::string getWaypoints();
    
    float getAltitude();

    int getCurrentWaypointIndex();

    void setMissionName(const std::string& mission_name);




};