#pragma once
#include <string>
#include <vector>



class Vehicle{ //No public setters for battery or status directly

protected:
    std::string status; //only settable through a method that validates allowed states and logs with timestamp
    std::vector<std::string> flight_log;
    std::string name;
    float battery_level;

public:

    Vehicle() = default;

    virtual ~Vehicle() = default; //Destructor
 
    void drain_battery(float amount);

    void charge_battery(float amount, int duration_seconds);

    bool is_critical();

      

    //Getter functions

    virtual void get_info() = 0; //purely virtual function
    
    std::string getName();    
    
    std::string getStatus() ;

    float getBattery_level();
    
    std::string getFlight_log(); //Need to think of the return type

    ///Setter Functions

    void setName(std::string name);
    void setStatus(const std::string& new_status);

    std::string get_timestamp();
};