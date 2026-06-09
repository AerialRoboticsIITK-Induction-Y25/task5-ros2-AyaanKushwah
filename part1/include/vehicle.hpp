#pragma once
#include <string>
#include <vector>



class Vehicle{ //No public setters for battery or status directly

private:
    std::string status; //only settable through a method that validates allowed states and logs with timestamp
    std::vector<std::string> flight_log;
    float battery_level;

protected:
    std::string name;
    void set_status(const std::string& new_status);
    void set_battery_level(const float& battery_level);

public:

    Vehicle(const std::string& name, const float& battery_level,const std::string& status);

    Vehicle() = default;

    virtual ~Vehicle() = default; //Destructor
 
    void drain_battery(float amount);

    void charge_battery(float amount, int duration_seconds);

    bool is_critical();

      
    //Getter functions

    virtual std::string get_info() = 0; //purely virtual function
    
    std::string get_name();    
    
    std::string get_status() ;

    float get_battery_level();
    
    std::string get_flight_log(); //Need to think of the return type

    ///Setter Functions

    void set_name(std::string name);
    
    std::vector<std::string> get_flight_vector();

    std::string get_timestamp();

    void add_log(const std::string& entry);

};