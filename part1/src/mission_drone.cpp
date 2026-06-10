#include "../include/mission_drone.hpp"
#include <cmath>
using namespace std;
#include <iostream>
#include <chrono>



    //Constructor
    MissionDrone::MissionDrone() = default;

    MissionDrone::MissionDrone(const std::string& drone_name, const float& battery_level, const float& max_altitude, const std::string& mission_name,  const std::vector<std::tuple<float, float, float>>& waypoints){
        current_waypoint_index = 0;

        this->name = drone_name;
        set_battery_level(battery_level);
        this->max_altitude = max_altitude;
        this->mission_name = mission_name;
        this->waypoints = waypoints;
    }

    MissionDrone::~MissionDrone() = default;

    tuple<float, float,float> MissionDrone::next_waypoint(){
        drain_battery(1.5);

        add_log(get_timestamp() + ": Moved to Waypoint " + to_string(current_waypoint_index + 1));
        visited_waypoints.emplace_back(waypoints[current_waypoint_index], "Checkpoint " +  to_string(current_waypoint_index) + "\n");

            if(current_waypoint_index + 1 < (int)waypoints.size()){
        auto waypointVector = std::make_tuple(
            get<0>(waypoints[current_waypoint_index+1]) - get<0>(waypoints[current_waypoint_index]),
            get<1>(waypoints[current_waypoint_index+1]) - get<1>(waypoints[current_waypoint_index]),
            get<2>(waypoints[current_waypoint_index+1]) - get<2>(waypoints[current_waypoint_index])
        );

        float distance = std::sqrt(
            pow(get<0>(waypointVector), 2) + 
            pow(get<1>(waypointVector), 2) + 
            pow(get<2>(waypointVector), 2)
        );

        if(distance > 0.001f){
            set_x_speed(get_speed() * get<0>(waypointVector) / distance);
            set_y_speed(get_speed() * get<1>(waypointVector) / distance);
            set_z_speed(get_speed() * get<2>(waypointVector) / distance);
        }
    }

        return waypoints[current_waypoint_index++];
    }

    void MissionDrone::skip_waypoint(const std::string& reason){
        add_log(get_timestamp() + ": Skipped Waypoint " + to_string(current_waypoint_index));
        (void)reason;
        current_waypoint_index ++;
    }

    bool MissionDrone::mission_complete(){
        return (current_waypoint_index == (int)waypoints.size()-1);
    }

    string MissionDrone::mission_summary(){ //Need to work on this
        return get_flight_log();
    }

    string MissionDrone::get_mission_name(){
        return mission_name;
    }

    string MissionDrone::get_visited_waypoints(){
    string result;
    for(const auto& it : visited_waypoints){
        result += it.second + ": (" + to_string(get<0>(it.first)) + "," + to_string(get<1>(it.first)) + "," + to_string(get<2>(it.first)) + ")\n"; 
    }

    return result;
}

    string MissionDrone::get_waypoints(){
        string result;
    for(const auto& it : waypoints){
        result += "(" + to_string(get<0>(it)) + "," + to_string(get<1>(it)) + "," + to_string(get<2>(it)) + ")\n"; 
    }

    return result;
    }

std::string MissionDrone::get_info(){
    std::string result;

    result = "Name: " + get_name() + "\n-----------------------------\n"
            + "Battery Level: " + to_string(get_battery_level()) + "\n-----------------------------\n"
            + "Max Altitude: " + to_string(get_battery_level()) + "\n-----------------------------\n"
            + "Status: " + get_status() + "\n-----------------------------\n"
            + "Mission Name: " + get_mission_name() + "\n-----------------------------\n"
            + "Waypoints: " + "\n"
            + get_waypoints();

            return result;
}


    int MissionDrone::get_current_waypoint_index(){
        return current_waypoint_index;
    }

    float MissionDrone::get_altitude(){
        return get<2>(waypoints[current_waypoint_index]);
    }

    void MissionDrone::set_mission_name(const std::string& mission_name){
        this->mission_name = mission_name;
    }

        



