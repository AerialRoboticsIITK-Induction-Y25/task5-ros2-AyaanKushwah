#include "../include/drone.hpp"
#include <iostream>
#include "../include/drone_exceptions.hpp"
#include <chrono>
using namespace std;





Drone::Drone(const string& name, const float& battery_level, const float& max_altitude){ //Custom Constructor
    this->max_altitude = max_altitude;
    this->name = name;
    this->battery_level = battery_level;
    setStatus("idle");
}

Drone::Drone() = default; //Default constructor

Drone::~Drone() = default; //Default destructor

void Drone::setSpeed(const float& speed){
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

    setStatus("flying");
    altitude = target_altitude;
    setSpeed(10.0f);
}

void Drone::land(){
    setStatus("idle");
    altitude = 0;
    setSpeed(0.0f);

}

void Drone::emergency_stop(){
    //initiate emergency stop
    //velocity = 0
    if(battery_level >= 30.0){
        drain_battery(30);
    } else {
        drain_battery(getBattery_level());
    }

    land();
}

float Drone::getSpeed(){
    return speed;
}

void Drone::get_info(){
    cout << "Name: " << getName();
    cout << "\n-----------------------------\n";

    cout << "Battery Level: " << getBattery_level();
    cout << "\n-----------------------------\n";
    cout << "Max Altitude: " << this-> max_altitude;
    cout << "\n-----------------------------\n";
    
    cout << "Status: " << getStatus();
    cout << "\n-----------------------------\n";
}


void Drone::setXSpeed(const float& Xspeed){
    this->v_x = Xspeed;

}

void Drone::setYSpeed(const float& Yspeed){
    this->v_y = Yspeed;
}

void Drone::setZSpeed(const float& Zspeed){
    this->v_z = Zspeed;
}

float Drone::getAltitude(){
    return this->altitude;
}



