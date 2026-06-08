#include <chrono>
#include <memory>
#include <string>
#include <sstream>
#include <iomanip>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "mission_drone.hpp"
#include "drone_exceptions.hpp"

using namespace std::chrono_literals;

class DroneNode : public rclcpp::Node {
private:
    rclcpp::TimerBase::SharedPtr status_timer_;
    rclcpp::TimerBase::SharedPtr telemetry_timer_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr status_pub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr alert_pub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr mission_pub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr telemetry_pub_;

    std::unique_ptr<MissionDrone> drone_;
    std::string drone_name_;
    int publish_count_ = 0;

public:
    DroneNode() : Node("drone_node") {

        // ── step 1: parameters ────────────────────────────────
        this->declare_parameter<std::string>("drone_name", "Alpha");
        this->declare_parameter<double>("initial_battery", 100.0);
        this->declare_parameter<std::string>("mission_name", "Default_Mission");

        drone_name_ = this->get_parameter("drone_name").as_string();
        double initial_battery = this->get_parameter("initial_battery").as_double();
        std::string mission_name = this->get_parameter("mission_name").as_string();

        // ── step 2: create MissionDrone ───────────────────────
        std::vector<std::tuple<float,float,float>> waypoints = {
            {5.0f,   10.0f,  15.0f},
            {10.0f,  10.0f,  10.0f},
            {10.0f,  10.0f, 15.0f},
            {20.0f,   15.0f, 20.0f},
            {15.0f,   25.0f,  35.0f}
        };

        drone_ = std::make_unique<MissionDrone>(
            drone_name_,
            static_cast<float>(initial_battery),
            100.0f,
            mission_name,
            waypoints
        );

        // ── step 3: take off ──────────────────────────────────
        try {
            drone_->take_off(15.0f);
            RCLCPP_INFO(this->get_logger(), "Drone [%s] took off successfully!", drone_name_.c_str());
        }
        catch(const AltitudeException& e){
            RCLCPP_ERROR(this->get_logger(), "Takeoff failed: %s", e.what());
        }

        // ── step 4: publishers ────────────────────────────────
        status_pub_    = this->create_publisher<std_msgs::msg::String>(
            "/drone/" + drone_name_ + "/status", 10);
        alert_pub_     = this->create_publisher<std_msgs::msg::String>(
            "/drone/" + drone_name_ + "/alert", 10);
        mission_pub_   = this->create_publisher<std_msgs::msg::String>(
            "/drone/" + drone_name_ + "/mission_complete", 10);
        telemetry_pub_ = this->create_publisher<std_msgs::msg::String>(
            "/drone/" + drone_name_ + "/telemetry", 10);

        // ── step 5: timers ────────────────────────────────────
        status_timer_    = this->create_wall_timer(
            1s, std::bind(&DroneNode::status_timer_callback, this));
        telemetry_timer_ = this->create_wall_timer(
            2s, std::bind(&DroneNode::telemetry_timer_callback, this));

        RCLCPP_INFO(this->get_logger(), 
            "Drone Node [%s] started. Battery: %.1f%%", 
            drone_name_.c_str(), drone_->getBattery_level());
    }

private:
    void status_timer_callback() {

        // stop publishing if landed
        if(drone_->getStatus() == "landed"){
            return;
        }

        // ── drain battery ─────────────────────────────────────
        try {
            drone_->drain_battery(0.5f);
        }
        catch(const BatteryDepletedException& e){
            RCLCPP_ERROR(this->get_logger(), 
                "Battery depleted: %s", e.what());
            drone_->land();
            return;
        }

        publish_count_++;

        // ── advance waypoint every 3 publishes ────────────────
        if(publish_count_ % 3 == 0){
            if(!drone_->mission_complete()){
                try {
                    drone_->next_waypoint();
                    RCLCPP_INFO(this->get_logger(), 
                        "Advanced to waypoint %d", 
                        drone_->getCurrentWaypointIndex());
                }
                catch(const BatteryDepletedException& e){
                    RCLCPP_ERROR(this->get_logger(), "%s", e.what());
                    drone_->land();
                    return;
                }
            }
        }

        // ── check critical battery ────────────────────────────
        if(drone_->is_critical()){
            std_msgs::msg::String alert_msg;
            alert_msg.data = "CRITICAL BATTERY: " + drone_name_ +
                             " | Battery: " +
                             std::to_string(drone_->getBattery_level()) + "%";
            alert_pub_->publish(alert_msg);
            RCLCPP_WARN(this->get_logger(), 
                "CRITICAL BATTERY! Landing drone [%s]", drone_name_.c_str());
            drone_->land();
            return;
        }

        // ── check mission complete ────────────────────────────
        if(drone_->mission_complete()){
            std_msgs::msg::String complete_msg;
            complete_msg.data = "MISSION COMPLETE: " + drone_name_;
            mission_pub_->publish(complete_msg);
            RCLCPP_INFO(this->get_logger(), 
                "Mission complete! Restarting for drone [%s]", 
                drone_name_.c_str());

            // restart mission
            publish_count_ = 0;
        }

        // ── build and publish status message ──────────────────
        std_msgs::msg::String status_msg;
        std::ostringstream ss;
        ss << "name:"     << drone_->getName()
           << "|battery:" << std::fixed << std::setprecision(1) 
                          << drone_->getBattery_level()
           << "|altitude:"<< std::fixed << std::setprecision(1) 
                          << drone_->getAltitude()
           << "|status:"  << drone_->getStatus()
           << "|waypoint:"<< drone_->getCurrentWaypointIndex() << "/5"
           << "|speed:"   << std::fixed << std::setprecision(1) 
                          << drone_->getSpeed();

        status_msg.data = ss.str();
        status_pub_->publish(status_msg);

        RCLCPP_INFO(this->get_logger(), "%s", status_msg.data.c_str());
    }

    void telemetry_timer_callback() {

        if(drone_->getStatus() == "landed") return;

        std::ostringstream json;
        json << "{"
             << "\"name\":\""    << drone_->getName()          << "\","
             << "\"battery\":"   << drone_->getBattery_level() << ","
             << "\"altitude\":"  << drone_->getAltitude()      << ","
             << "\"status\":\""  << drone_->getStatus()        << "\","
             << "\"waypoint\":"  << drone_->getCurrentWaypointIndex() << ","
             << "\"speed\":"     << drone_->getSpeed()         << ","
             << "\"mission\":\""  << drone_->getMissionName()  << "\","
             << "\"critical\":"  << (drone_->is_critical() ? "true" : "false")
             << "}";

        std_msgs::msg::String telemetry_msg;
        telemetry_msg.data = json.str();
        telemetry_pub_->publish(telemetry_msg);
    }
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DroneNode>());
    rclcpp::shutdown();
    return 0;
}