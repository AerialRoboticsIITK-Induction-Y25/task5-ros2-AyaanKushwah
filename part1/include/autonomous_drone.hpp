    #pragma once
    #include "mission_drone.hpp"
    #include <vector>
    #include <string>

    class AutonomousDrone : public MissionDrone{
    private:
        std::string ai_mode;
        std::tuple<float, float, float> home_position;
        std::vector<std::string> obstacle_log;
        std::vector<std::tuple<float, float, float>> obstacles;

    public:

        AutonomousDrone();
    
        AutonomousDrone(const std::string& drone_name, const float& battery_level, const float& max_altitude, const std::string& mission_name, std::vector<std::tuple<float, float, float>> auto_waypoints, const std::string& ai_mode, const std::tuple<float, float, float>& home_position);
        void set_ai_mode(const std::string& mode);
        void detect_obstacle(std::tuple<float,float,float> position, const std::string& severity);
        std::vector<std::tuple<float,float,float>>  auto_replan(const std::vector<std::tuple<float,float,float>>& obstacles);
        void get_info() override;

        std::string getMode();
        std::tuple<float, float, float> getHomePosition();
        std::string getObstacleLog();
        void setWaypoints(const std::vector<std::tuple<float, float, float>>& waypoints);

        ~AutonomousDrone() = default;

    };