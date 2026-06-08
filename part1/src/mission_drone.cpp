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
        this->battery_level = battery_level;
        this->max_altitude = max_altitude;
        this->mission_name = mission_name;
        this->waypoints = waypoints;
    }

    MissionDrone::~MissionDrone() = default;

    tuple<float, float,float> MissionDrone::next_waypoint(){
        drain_battery(1.5);
        flight_log.push_back(get_timestamp() + ": Moved to Waypoint " + to_string(current_waypoint_index + 1));
        visited_waypoints.emplace_back(waypoints[current_waypoint_index], "Checkpoint " +  to_string(current_waypoint_index) + "\n");

        tuple waypointVector = std::make_tuple(
            get<0>(waypoints[current_waypoint_index+1]) - get<0>(waypoints[current_waypoint_index]),
            get<1>(waypoints[current_waypoint_index+1]) - get<1>(waypoints[current_waypoint_index]),
            get<2>(waypoints[current_waypoint_index+1]) - get<2>(waypoints[current_waypoint_index])
        );

        //Calculating the distance between the current and the next waypoint
        int distance = std::sqrt(
            pow(get<0>(waypointVector), 2) + 
            pow(get<1>(waypointVector), 2) + 
            pow(get<2>(waypointVector), 2)
        );

        tuple unitVector= std::make_tuple(
            get<0>(waypointVector) / distance,
            get<1>(waypointVector) / distance,
            get<2>(waypointVector) / distance);

        //Calculating the component of velocities
        setXSpeed(getSpeed()*get<0>(unitVector));
        setYSpeed(getSpeed()*get<1>(unitVector));
        setZSpeed(getSpeed()*get<2>(unitVector));

        return waypoints[current_waypoint_index++];
    }

    void MissionDrone::skip_waypoint(const std::string& reason){
        flight_log.push_back(get_timestamp() + ": Skipped Waypoint " + to_string(current_waypoint_index));
        (void)reason;
        current_waypoint_index ++;
    }

    bool MissionDrone::mission_complete(){
        return (current_waypoint_index == (int)waypoints.size()-1);
    }

    string MissionDrone::mission_summary(){ //Need to work on this
        
        string result;

        for(const auto& it : flight_log)
            result += it + "\n";

        return result;
    }

    string MissionDrone::getMissionName(){
        return mission_name;
    }

    string MissionDrone::getVisitedWaypoints(){
    string result;
    for(const auto& it : visited_waypoints){
        result += it.second + ": (" + to_string(get<0>(it.first)) + "," + to_string(get<1>(it.first)) + "," + to_string(get<2>(it.first)) + ")\n"; 
    }

    return result;
}

    string MissionDrone::getWaypoints(){
        string result;
    for(const auto& it : waypoints){
        result += "(" + to_string(get<0>(it)) + "," + to_string(get<1>(it)) + "," + to_string(get<2>(it)) + ")\n"; 
    }

    return result;
    }

    void MissionDrone::get_info(){
        cout << "Drone Name: " << getName();
        cout << "\n-----------------------------\n";
        cout << "Battery Level: " << getBattery_level();
        cout << "\n-----------------------------\n";
        cout << "Max Altitude: " << this->max_altitude;
        cout << "\n-----------------------------\n";
        cout << "Mission Name: " << getMissionName();
        cout << "\n-----------------------------\n";
        cout << "Waypoints: \n" << getWaypoints();
        cout << "\n-----------------------------\n";
        
    }


    int MissionDrone::getCurrentWaypointIndex(){
        return current_waypoint_index;
    }

    float MissionDrone::getAltitude(){
        return get<2>(waypoints[current_waypoint_index]);
    }

    void MissionDrone::setMissionName(const std::string& mission_name){
        this->mission_name = mission_name;
    }

        



