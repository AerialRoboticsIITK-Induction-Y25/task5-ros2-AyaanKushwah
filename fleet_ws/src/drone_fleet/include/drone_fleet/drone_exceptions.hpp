#pragma once
#include <string>
#include <iostream>

class DroneException : public std::exception{

private:
    std::string message;
    std::string drone_name;

public:
    DroneException(const char* msg, const std::string& name) 
    : message(msg), drone_name(name) {}

    const char* what() const noexcept {
    return message.c_str();
    }

    const std::string& getDroneName() const {
        return drone_name;
    }

};


class BatteryDepletedException : public DroneException {
private:
    float battery_level;

public:
    BatteryDepletedException(const char* msg, const std::string& name, float level)
        : DroneException(msg, name), battery_level(level) {}

    float getBatteryLevel() const {
        return battery_level;
    }
};


class InvalidStateException : public DroneException {
private:
    std::string current_state;

public:
    InvalidStateException(const char* msg, const std::string& name, const std::string& state)
        : DroneException(msg, name), current_state(state) {}

    const std::string& getCurrentState() const {
        return current_state;
    }
};

class AltitudeException : public DroneException {
private:
    float attempted_altitude;
    float max_altitude;

public:
    AltitudeException(const char* msg, const std::string& name, float attempted, float max)
        : DroneException(msg, name), attempted_altitude(attempted), max_altitude(max) {}

    float getAttemptedAltitude() const { return attempted_altitude; }
    float getMaxAltitude() const { return max_altitude; }
};