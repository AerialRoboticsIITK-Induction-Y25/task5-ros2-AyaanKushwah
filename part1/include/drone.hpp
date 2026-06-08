#pragma once
#include "vehicle.hpp"
#include <cmath>

class Drone : public Vehicle {

private:
    float speed;
    float v_x, v_y, v_z;

protected:
    float altitude;
    float max_altitude;

public:

    Drone(const std::string& name, const float& battery_level , const float& max_altitude);
    Drone();

    ~Drone();

    void take_off(float target_altitude); //thows AltitudeError if limit exceeded

    void land(); //velocity down i guess,lowering speed etc, if velocity = 0 then status = landed

    void emergency_stop(); //Cuts 30 battery as penalty

    void get_info() override; //override it

    float getSpeed();

    float getAltitude();

public:
    void setSpeed(const float& speed);
    void setXSpeed(const float& Xspeed);
    void setYSpeed(const float& Yspeed);
    void setZSpeed(const float& Zspeed);
};