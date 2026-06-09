#include "../include/drone.hpp"
#include <iostream>
#include "../include/drone_exceptions.hpp"
#include <chrono>
using namespace std;





Drone::Drone(const std::string& name, const float& battery_level,const std::string& status, const float& max_altitude)
    : Vehicle::Vehicle(name, battery_level, status),   // initializer list
      max_altitude(max_altitude)      // same list, not this->
{
    // constructor body — empty or add logic here
}

Drone::Drone() = default; //Default constructor

Drone::~Drone() = default; //Default destructor

void Drone::set_speed(const float& speed){
    this->speed = speed;
}



void Drone::take_off(float target_altitude){
    if(target_altitude > max_altitude){
        throw AltitudeException("Altitude limit exceeded!", name, target_altitude, max_altitude);

        return;
    } else if (is_critical() == true){
        cout << "Can't take off at critical situation!";
        return;
    }

    set_status("flying");
    altitude = target_altitude;
    set_speed(10.0f);
}

void Drone::land(){
    set_status("idle");
    altitude = 0;
    set_speed(0.0f);

}

void Drone::emergency_stop(){
    //initiate emergency stop
    //velocity = 0
    if(get_battery_level() >= 30.0){
        drain_battery(30);
    } else {
        drain_battery(get_battery_level());
    }

    land();
}

float Drone::get_speed(){
    return speed;
}

std::string Drone::get_info(){
    std::string result;

    result = "Name: " + get_name() + "\n-----------------------------\n"
            + "Battery Level: " + to_string(get_battery_level()) + "\n-----------------------------\n"
            + "Max Altitude: " + to_string(get_battery_level()) + "\n-----------------------------\n"
            + "Status: " + get_status() + "\n-----------------------------\n";

            return result;
}


void Drone::set_x_speed(const float& Xspeed){
    this->v_x = Xspeed;

}

void Drone::set_y_speed(const float& Yspeed){
    this->v_y = Yspeed;
}

void Drone::set_z_speed(const float& Zspeed){
    this->v_z = Zspeed;
}

float Drone::get_altitude(){
    return this->altitude;
}



