#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_srvs/srv/trigger.hpp"
#include <map>
#include <string>
#include <iostream>

struct TelemetryData {
    std::string name = "Unknown";
    double battery = 0.0;
    double altitude = 0.0;
    std::string status = "Unknown";
    int waypoint = 0;
};
    
class FleetManager : public rclcpp::Node {
public:
    FleetManager() : Node("fleet_manager") {
        std::vector<std::string> drones = {"Alpha", "Beta", "Gamma"};
        
        for (const auto& name : drones) {
            fleet_store_[name] = TelemetryData{name, 0.0, 0.0, "Offline", 0};

            std::string prefix = "/drone/" + name;
            
            telemetry_subs_[name] = this->create_subscription<std_msgs::msg::String>(
                prefix + "/telemetry", 10, [this, name](const std_msgs::msg::String::SharedPtr msg) {
                    parse_telemetry_json(msg->data, name);
                });

            alert_subs_[name] = this->create_subscription<std_msgs::msg::String>(
                prefix + "/alert", 10, [this](const std_msgs::msg::String::SharedPtr msg) {
                    RCLCPP_WARN(this->get_logger(), "[ALERT RECEIVED] %s", msg->data.c_str());
                });
        }

        report_timer_ = this->create_wall_timer(std::chrono::seconds(5), std::bind(&FleetManager::print_report, this));

        srv_ = this->create_service<std_srvs::srv::Trigger>(
            "/fleet/status_report", std::bind(&FleetManager::handle_trigger, this, std::placeholders::_1, std::placeholders::_2));
    }

private:
    std::string extract_value(const std::string& json, const std::string& key) {
        size_t pos = json.find("\"" + key + "\"");
        if (pos == std::string::npos) return "";
        size_t start = json.find(":", pos) + 1;
        while(json[start] == ' ' || json[start] == '"') start++;
        size_t end = start;
        while(json[end] != ',' && json[end] != '}' && json[end] != '"') end++;
        return json.substr(start, end - start);
    }

    void parse_telemetry_json(const std::string& raw_json, const std::string& name) {
        try {
            fleet_store_[name].battery = std::stod(extract_value(raw_json, "battery"));
            fleet_store_[name].altitude = std::stod(extract_value(raw_json, "altitude"));
            fleet_store_[name].status = extract_value(raw_json, "status");
            fleet_store_[name].waypoint = std::stoi(extract_value(raw_json, "waypoint"));
        } catch(...) {}
    }

    void print_report() {
        std::cout << "\n================= FLEET REPORT =================" << std::endl;
        std::cout << "Drone\t| Battery\t| Altitude\t| Waypoint\t| Status" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
        for (const auto& [name, data] : fleet_store_) {
            std::cout << name << "\t| " << data.battery << "%\t| " 
                      << data.altitude << "m\t| " << data.waypoint << "/5\t\t| " << data.status << std::endl;
        }
        std::cout << "================================================" << std::endl;
    }

    void handle_trigger(const std::shared_ptr<std_srvs::srv::Trigger::Request>,
                        std::shared_ptr<std_srvs::srv::Trigger::Response> res) {
        print_report();
        res->success = true;
        res->message = "Fleet report generated dynamically on terminal log output.";
    }

    std::map<std::string, TelemetryData> fleet_store_;
    std::map<std::string, rclcpp::Subscription<std_msgs::msg::String>::SharedPtr> telemetry_subs_;
    std::map<std::string, rclcpp::Subscription<std_msgs::msg::String>::SharedPtr> alert_subs_;
    rclcpp::TimerBase::SharedPtr report_timer_;
    rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr srv_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<FleetManager>());
    rclcpp::shutdown();
    return 0;
}