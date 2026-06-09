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

// drone.hpp — declaration only, no body
Drone(const std::string& name, const float& battery_level,
      const std::string& status, const float& max_altitude);  // no {} here
    Drone();

    ~Drone();

    void take_off(float target_altitude); //thows AltitudeError if limit exceeded

    void land(); //velocity down i guess,lowering speed etc, if velocity = 0 then status = landed

    void emergency_stop(); //Cuts 30 battery as penalty

    std::string get_info() override; //override it

    float get_speed();

    float get_altitude();

public:
    void set_speed(const float& speed);
    void set_x_speed(const float& Xspeed);
    void set_y_speed(const float& Yspeed);
    void set_z_speed(const float& Zspeed);
};