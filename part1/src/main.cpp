#include <iostream>
#include <vector>
#include <tuple>
#include <random>
#include <cmath>
#include "../include/vehicle.hpp"
#include "../include/drone.hpp"
#include "../include/mission_drone.hpp"
#include "../include/autonomous_drone.hpp"
#include "../include/drone_exceptions.hpp"

using namespace std;

int main(){

    // ── SETUP ─────────────────────────────────────────────────

    vector<tuple<float,float,float>> mission_waypoints = {
        {10.0f, 0.0f,  5.0f},
        {20.0f, 10.0f, 10.0f},
        {30.0f, 20.0f, 15.0f},
        {20.0f, 30.0f, 10.0f},
        {10.0f, 20.0f, 5.0f}
    };

    vector<tuple<float,float,float>> auto_waypoints = {
        {0.0f,  10.0f,  15.0f},
        {15.0f, 5.0f,  10.0f},
        {25.0f, 15.0f, 15.0f},
        {15.0f, 25.0f, 10.0f},
        {30.0f,  15.0f,  10.0f}
    };

    // create one object of each class with actual type pointers
    Drone* drone = new Drone(
        "Alpha",    // name
        100.0f,
        "idle",     // battery
        120.0f      // max_altitude
    );

    MissionDrone* mission = new MissionDrone(
        "Beta",             // name
        80.0f,              // battery
        120.0f,             // max_altitude
        "Mission1",         // mission_name
        mission_waypoints   // waypoints
    );

    AutonomousDrone* autoDrone = new AutonomousDrone(
        "Gamma",             // name
        90.0f,               // battery
        120.0f,              // max_altitude
        "AutoMission",       // mission_name
        auto_waypoints,      // waypoints
        "auto",              // ai_mode
        {0.0f, 0.0f, 0.0f}  // home_position
    );

    // ── PART 1: POLYMORPHISM ───────────────────────────────────

    cout << "\n========== POLYMORPHISM DEMO ==========" << endl;

    // store all in Vehicle* vector
    vector<Vehicle*> drones;
    drones.push_back(drone);
    drones.push_back(mission);
    drones.push_back(autoDrone);

    // get_info() calls correct version for each object
    for(auto& it : drones){
    cout << it->get_info() << endl;
    cout << "──────────────────────────────" << endl;
}

    // ── PART 2: PRIVATE MEMBER ACCESS ─────────────────────────

    cout << "\n========== PRIVATE MEMBER ACCESS ==========" << endl;

    // private members cannot be accessed directly:
    // cout << drone->battery_level;   // ❌ ERROR: 'battery_level' is private
    // cout << drone->speed;           // ❌ ERROR: 'speed' is private
    // cout << drone->status;          // ❌ ERROR: 'status' is protected
    // cout << drone->max_altitude;    // ❌ ERROR: 'max_altitude' is protected
    // must use getters:
    
    cout << "Name:     " << drone->get_name()          << endl;
    cout << "Battery:  " << drone->get_battery_level() << "%" << endl;
    cout << "Status:   " << drone->get_status()        << endl;
    cout << "Speed:    " << drone->get_speed()         << endl;
    cout << "Altitude: " << drone->get_altitude()      << "m" << endl;

    // ── PART 3: EXCEPTION HANDLING ────────────────────────────

    cout << "\n========== EXCEPTION HANDLING ==========" << endl;

    // drain_battery — normal drain
    cout << "\n--- drain_battery() normal ---" << endl;
    try {
        drone->drain_battery(30.0f);
        cout << "Battery after drain: " << drone->get_battery_level() << "%" << endl;
    }
    catch(const BatteryDepletedException& e){
        cout << "Caught BatteryDepletedException!" << endl;
        cout << "  Message: " << e.what()           << endl;
        cout << "  Drone:   " << e.getDroneName()   << endl;
        cout << "  Level:   " << e.getBatteryLevel()<< "%" << endl;
    }

    // drain_battery — throw when depleted
    cout << "\n--- drain_battery() depleted ---" << endl;
    try {
        drone->drain_battery(999.0f);  // drains to 0
        drone->drain_battery(1.0f);    // throws
    }
    catch(const BatteryDepletedException& e){
        cout << "Caught BatteryDepletedException!" << endl;
        cout << "  Message: " << e.what()           << endl;
        cout << "  Drone:   " << e.getDroneName()   << endl;
        cout << "  Level:   " << e.getBatteryLevel()<< "%" << endl;
    }

    // take_off — throw when altitude exceeded
    cout << "\n--- take_off() altitude exceeded ---" << endl;
    try {
        mission->take_off(9999.0f);
    }
    catch(const AltitudeException& e){
        cout << "Caught AltitudeException!" << endl;
        cout << "  Message:   " << e.what()                  << endl;
        cout << "  Drone:     " << e.getDroneName()          << endl;
        cout << "  Attempted: " << e.getAttemptedAltitude()  << "m" << endl;
        cout << "  Max:       " << e.getMaxAltitude()        << "m" << endl;
    }

    // charge_battery — throw when not in charging state
    cout << "\n--- charge_battery() invalid state ---" << endl;
    try {
        mission->charge_battery(2.0f, 10);  // status not "charging" → throws
    }
    catch(const InvalidStateException& e){
        cout << "Caught InvalidStateException!" << endl;
        cout << "  Message: " << e.what()              << endl;
        cout << "  Drone:   " << e.getDroneName()      << endl;
        cout << "  State:   " << e.getCurrentState()   << endl;
    }

    // catch via base DroneException
    cout << "\n--- catching via base DroneException ---" << endl;
    try {
        drone->drain_battery(1.0f);  // battery already 0 → throws
    }
    catch(const DroneException& e){
        cout << "Caught via base DroneException!" << endl;
        cout << "  Message: " << e.what()         << endl;
        cout << "  Drone:   " << e.getDroneName() << endl;
    }

    // detect_obstacle
    cout << "\n--- detect_obstacle() demo ---" << endl;
    try {
        autoDrone->detect_obstacle({10.0f, 5.0f, 10.0f}, "high");
    }
    catch(const DroneException& e){
        cout << "Caught from detect_obstacle: " << e.what() << endl;
    }

    // ── PART 4: FULL AUTONOMOUS MISSION ───────────────────────

    cout << "\n========== FULL AUTONOMOUS MISSION ==========" << endl;

    // generate random obstacles
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> randDist(0.0f, 30.0f);

    vector<tuple<float,float,float>> obstacles;
    for(int i = 0; i < 3; i++){
        obstacles.push_back({randDist(gen), randDist(gen), randDist(gen)});
    }

    cout << "\nGenerated obstacles:" << endl;
    for(auto& [x, y, z] : obstacles){
        cout << "  (" << x << ", " << y << ", " << z << ")" << endl;
    }

    // take off
    cout << "\n--- Taking off ---" << endl;
    try {
        autoDrone->take_off(15.0f);
        cout << "Took off! Altitude: " << autoDrone->get_altitude() << "m" << endl;
    }
    catch(const AltitudeException& e){
        cout << "Takeoff failed: " << e.what() << endl;
    }

    // set ai mode
    autoDrone->set_ai_mode("auto");

    // iterate all waypoints
    cout << "\n--- Running mission ---" << endl;
    bool replanned = false;

    while(!autoDrone->mission_complete()){
        try {
            auto current_pos = autoDrone->next_waypoint();

            cout << "At waypoint " << autoDrone->get_current_waypoint_index()
                 << ": (" << get<0>(current_pos) << ", "
                 << get<1>(current_pos) << ", "
                 << get<2>(current_pos) << ")" << endl;

            // check each obstacle against current position
            for(auto& obs : obstacles){
                float dx = get<0>(current_pos) - get<0>(obs);
                float dy = get<1>(current_pos) - get<1>(obs);
                float dz = get<2>(current_pos) - get<2>(obs);
                float dist_to_obs = sqrt(dx*dx + dy*dy + dz*dz);

                if(dist_to_obs < 5.0f && !replanned){
                    cout << "  Obstacle within 5m! Initiating replan..." << endl;

                    // simulate high severity obstacle
                    try {
                        autoDrone->detect_obstacle(obs, "high");
                    }
                    catch(const DroneException& e){
                        cout << "  Emergency stop triggered: " << e.what() << endl;
                    }

                    // replan path
                    auto new_waypoints = autoDrone->auto_replan(obstacles);
                    autoDrone->set_waypoints(new_waypoints);
                    replanned = true;

                    cout << "  Path replanned! New waypoint count: "
                         << new_waypoints.size() << endl;
                    break;
                }
            }
        }
        catch(const BatteryDepletedException& e){
            cout << "Battery died during mission: " << e.what() << endl;
            autoDrone->land();
            break;
        }
    }

    // print mission summary
    cout << "\n--- Mission Summary ---" << endl;
    cout << autoDrone->mission_summary() << endl;

    // print flight log
    cout << "\n--- Flight Log ---" << endl;
    cout << autoDrone->get_flight_log() << endl;

    // ── CLEANUP ───────────────────────────────────────────────
    for(auto& it : drones)
        delete it;

    return 0;
}