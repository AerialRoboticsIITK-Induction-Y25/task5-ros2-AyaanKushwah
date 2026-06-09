#include "../include/vehicle.hpp"
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <thread>
#include "../include/drone_exceptions.hpp"

using namespace std;


Vehicle::Vehicle(const std::string& name, const float& battery_level,const std::string& status){
    this->name = name;
    this->battery_level = battery_level;
    setStatus(status);
}

//Method functions

std::string Vehicle::get_timestamp(){
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);

        // tm struct breaks time into components
        std::tm* tm_info = std::localtime(&time);

        char buffer[30];
        // format it however you want
        std::strftime(buffer, 30, "%Y-%m-%d %H:%M:%S", tm_info);
        return std::string(buffer);

}

void Vehicle::charge_battery(float ChargeRate, int duration_seconds)
{
    if(status != "charging")
        throw InvalidStateException("Not in charging state!", name, status);

    battery_level += ChargeRate * duration_seconds;

    if(battery_level > 100.0f)
        battery_level = 100.0f;

    flight_log.push_back(
        get_timestamp() + ": Charged for " + to_string(duration_seconds) + "s");

}

void Vehicle::drain_battery(float amount)
{
    if (battery_level <= 0)
        throw BatteryDepletedException("Battery dead!", name, battery_level);

    battery_level -= amount;

    if (battery_level < 0){
        battery_level = 0;
    }


    flight_log.push_back(get_timestamp() + ": Battery drained to " + to_string(battery_level));
}

bool Vehicle::is_critical(){
    return (battery_level < 30.0f);
    //check for emergency stop but what if battery < 30 ?
    //instantiate RTL
}

//Getter functions

string Vehicle::getName(){
    return name;
}

string Vehicle::getStatus(){
    return status;
}

float Vehicle::getBattery_level(){
    return battery_level;
}

std::vector<std::string> Vehicle::getFlightVector(){
    return flight_log;
}

string Vehicle::getFlight_log()
{
    string result;

    for(const auto& it : flight_log){
        result += it + "\n";
    }

    return result;
}
//Setter functions

void Vehicle::setName(string name){
    this->name = name;
}

void Vehicle::setStatus(const std::string& new_status){
    vector<string> allowed = {"idle", "flying", "charging"};
    
    // validate
    bool valid = false;
    for(auto& s : allowed)
        if(s == new_status){
            valid = true;
        } 
    
    if(!valid)
        throw InvalidStateException("Invalid status!", name, new_status);
    
    this->status = new_status;
    flight_log.push_back(get_timestamp() + ": Status changed to " + new_status);
}

void Vehicle::setBatteryLevel(const float& battery_level){
    this->battery_level = battery_level;
}

    void Vehicle::add_log(const std::string& entry) {
        flight_log.push_back(entry);  // only this method touches flight_log directly
    }

