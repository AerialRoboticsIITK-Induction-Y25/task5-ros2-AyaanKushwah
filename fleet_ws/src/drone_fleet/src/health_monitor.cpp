#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <map>
#include <deque>
#include <sstream>
#include <iomanip>

struct Sample {
    double battery;
    rclcpp::Time timestamp;
};

class HealthMonitor : public rclcpp::Node {
public:
    HealthMonitor() : Node("health_monitor") {
        std::vector<std::string> drones = {"Alpha", "Beta", "Gamma"};
        warning_pub_ = this->create_publisher<std_msgs::msg::String>("/fleet/health_warning", 10);
        summary_pub_ = this->create_publisher<std_msgs::msg::String>("/fleet/health_summary", 10);

        for (const auto& name : drones) {
            subs_[name] = this->create_subscription<std_msgs::msg::String>(
                "/drone/" + name + "/telemetry", 10,
                [this, name](const std_msgs::msg::String::SharedPtr msg) { process_telemetry(msg->data, name); });
        }

        diag_timer_ = this->create_wall_timer(std::chrono::seconds(10), std::bind(&HealthMonitor::print_diagnostics, this));
    }

private:
    double extract_double(const std::string& json, const std::string& key) {
        size_t pos = json.find("\"" + key + "\"");
        if (pos == std::string::npos) return 0.0;
        size_t start = json.find(":", pos) + 1;
        while(json[start] == ' ' || json[start] == '"') start++;
        return std::stod(json.substr(start));
    }

    void process_telemetry(const std::string& raw_json, const std::string& name) {
        double current_batt = extract_double(raw_json, "battery");
        rclcpp::Time now = this->now();

        auto& buffer = history_[name];
        buffer.push_back({current_batt, now});
        if (buffer.size() > 10) buffer.pop_front();

        if (buffer.size() >= 2) {
            double drop = buffer.front().battery - buffer.back().battery;
            double dt = (buffer.back().timestamp - buffer.front().timestamp).seconds();
            if (dt > 0.0) {
                double rate = drop / dt;
                if (rate > 1.5) {
                    auto warn = std_msgs::msg::String();
                    warn.data = "CRITICAL METRIC EXCEEDED: " + name + " drain rate is " + std::to_string(rate) + "/s!";
                    warning_pub_->publish(warn);
                }
            }
        }
    }

    void print_diagnostics() {
        std::cout << "\n================= HEALTH DIAGNOSTICS =================" << std::endl;
        std::cout << "Drone\t| Drain Rate/s\t| Time to Crit (20%)\t| Time to Depletion" << std::endl;
        std::cout << "------------------------------------------------------------" << std::endl;

        std::stringstream summary_json;
        summary_json << "{";

        for (auto const& [name, buffer] : history_) {
            double rate = 0.0;
            double t_crit = -1.0, t_dep = -1.0;
            if (buffer.size() >= 2) {
                double drop = buffer.front().battery - buffer.back().battery;
                double dt = (buffer.back().timestamp - buffer.front().timestamp).seconds();
                if (dt > 0.0) rate = drop / dt;
                
                double current_batt = buffer.back().battery;
                if (rate > 0.0) {
                    if (current_batt > 20.0) t_crit = (current_batt - 20.0) / rate;
                    else t_crit = 0.0;
                    t_dep = current_batt / rate;
                }
            }

            std::cout << name << "\t| " << std::fixed << std::setprecision(2) << rate << "\t\t| " 
                      << t_crit << "s\t\t\t| " << t_dep << "s" << std::endl;

            summary_json << "\"" << name << "\":{\"rate\":" << rate << ",\"t_crit\":" << t_crit << "},";
        }
        std::cout << "============================================================" << std::endl;
        
        std::string json_str = summary_json.str();
        if(json_str.back() == ',') json_str.pop_back();
        json_str += "}";
        auto summary_msg = std_msgs::msg::String();
        summary_msg.data = json_str;
        summary_pub_->publish(summary_msg);
    }

    std::map<std::string, std::deque<Sample>> history_;
    std::map<std::string, rclcpp::Subscription<std_msgs::msg::String>::SharedPtr> subs_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr warning_pub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr summary_pub_;
    rclcpp::TimerBase::SharedPtr diag_timer_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<HealthMonitor>());
    rclcpp::shutdown();
    return 0;
}