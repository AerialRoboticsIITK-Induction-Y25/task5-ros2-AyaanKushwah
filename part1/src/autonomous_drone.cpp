#include "../include/autonomous_drone.hpp"
#include "../include/mission_drone.hpp"
#include <string>
#include <tuple>
#include <iostream>
#include <chrono>
#include <string>
#include <cmath>
using namespace std;
    


    AutonomousDrone::AutonomousDrone(const std::string& drone_name, const float& battery_level, const float& max_altitude, const std::string& mission_name, std::vector<std::tuple<float, float, float>> auto_waypoints, const std::string& ai_mode, const std::tuple<float, float, float>& home_position){
        set_ai_mode(ai_mode);
        this->name = drone_name;
        setBatteryLevel(battery_level);
        this->max_altitude = max_altitude;
        setMissionName(mission_name);
        this->waypoints = auto_waypoints;
        this->home_position = home_position;
    }


    void AutonomousDrone::set_ai_mode(const string& mode){
        ai_mode = mode;
        if(mode == "return home"){
            waypoints.erase(waypoints.begin() + current_waypoint_index + 1, waypoints.end());
            waypoints.push_back(home_position);
        }
    }

    void AutonomousDrone::detect_obstacle(tuple<float,float,float> position, const string& severity){
        
        obstacle_log.push_back(get_timestamp() + "Obstacle detected at ()" + 
            to_string(get<0>(position))+ "," + to_string(get<1>(position)) + "," + to_string(get<2>(position)) + ") | Severity: "
            + severity);

        if(severity == "high"){
            emergency_stop();
        }
    }

    // helper function — put this above auto_replan in autonomousdrone.cpp
float point_to_segment_distance(
    std::tuple<float,float,float> W1,
    std::tuple<float,float,float> W2,
    std::tuple<float,float,float> X)
{
    float w1x = std::get<0>(W1), w1y = std::get<1>(W1), w1z = std::get<2>(W1);
    float w2x = std::get<0>(W2), w2y = std::get<1>(W2), w2z = std::get<2>(W2);
    float ox  = std::get<0>(X),  oy  = std::get<1>(X),  oz  = std::get<2>(X);

    float dx = w2x - w1x;
    float dy = w2y - w1y;
    float dz = w2z - w1z;
    float len_sq = dx*dx + dy*dy + dz*dz;

    if(len_sq < 0.001f)
        return sqrt(pow(ox-w1x,2) + pow(oy-w1y,2) + pow(oz-w1z,2));

    float t = ((ox-w1x)*dx + (oy-w1y)*dy + (oz-w1z)*dz) / len_sq;
    t = std::max(0.0f, std::min(1.0f, t));

    float cx = w1x + t * dx;
    float cy = w1y + t * dy;
    float cz = w1z + t * dz;

    return sqrt(pow(ox-cx,2) + pow(oy-cy,2) + pow(oz-cz,2));
}

// main auto_replan function
std::vector<std::tuple<float,float,float>> AutonomousDrone::auto_replan(
    const std::vector<std::tuple<float,float,float>>& obstacles)
{
    std::vector<std::tuple<float,float,float>> new_waypoints;
    new_waypoints.push_back(waypoints[0]);  // first waypoint stays

    for(int i = 1; i < (int)waypoints.size(); i++){
        float wx = std::get<0>(waypoints[i]);
        float wy = std::get<1>(waypoints[i]);
        float wz = std::get<2>(waypoints[i]);

        for(auto& obstacle : obstacles){
            float ox = std::get<0>(obstacle);
            float oy = std::get<1>(obstacle);
            float oz = std::get<2>(obstacle);

            // ── CASE 1: waypoint too close to obstacle ──────────
            float dx = wx - ox;
            float dy = wy - oy;
            float dz = wz - oz;
            float dist_to_waypoint = sqrt(dx*dx + dy*dy + dz*dz);

            if(dist_to_waypoint < 5.0f){
                float mag = dist_to_waypoint < 0.001f ? 1.0f : dist_to_waypoint;
                wx = ox + (dx/mag) * 5.5f;
                wy = oy + (dy/mag) * 5.5f;
                wz = oz + (dz/mag) * 5.5f;

                obstacle_log.push_back(get_timestamp() +
                    ": CASE1 - Waypoint shifted away from obstacle");
            }

            // ── CASE 2 & 3: obstacle in path / within 5m of segment ─
            float path_dist = point_to_segment_distance(
                new_waypoints.back(),
                {wx, wy, wz},
                obstacle
            );

            if(path_dist < 5.0f){
                // perpendicular direction to path
                float path_dx = wx - std::get<0>(new_waypoints.back());
                float path_dy = wy - std::get<1>(new_waypoints.back());
                float path_len = sqrt(path_dx*path_dx + path_dy*path_dy);

                float perp_x, perp_y;
                if(path_len < 0.001f){
                    // path has no direction — just offset in x
                    perp_x = 1.0f;
                    perp_y = 0.0f;
                } else {
                    // rotate 90 degrees in xy plane
                    perp_x = -path_dy / path_len;
                    perp_y =  path_dx / path_len;
                }

                // insert detour waypoint 6 units perpendicular to path
                float detour_x = ox + perp_x * 6.0f;
                float detour_y = oy + perp_y * 6.0f;
                float detour_z = wz;  // keep same altitude

                new_waypoints.push_back({detour_x, detour_y, detour_z});

                obstacle_log.push_back(get_timestamp() +
                    ": CASE2/3 - Detour waypoint inserted, obstacle in path");
            }
        }

        // add the (possibly shifted) waypoint
        new_waypoints.push_back({wx, wy, wz});
    }

    return new_waypoints;
}

    void AutonomousDrone::setWaypoints(const std::vector<std::tuple<float, float, float>>& waypoints){
        this->waypoints = waypoints;
    }

    void AutonomousDrone::get_info(){
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
        cout << "AI_Mode: " << getMode();
        cout << "\n-----------------------------\n";
        cout << "Home Position: " << "(" << to_string(get<0>(home_position)) << "," << to_string(get<1>(home_position)) << "," << to_string(get<2>(home_position)) << ")";
        cout << "\n-----------------------------\n";
        
    };

    string AutonomousDrone::getMode(){
        return ai_mode;
    }

    tuple<float, float, float> AutonomousDrone::getHomePosition(){
        return home_position;
    }

    string AutonomousDrone::getObstacleLog(){
        string result;

        for(const auto& it : obstacle_log)
            result += it + "\n";

        return result;
}
