#pragma once

#include <string>
#include <vector>

struct InputConfig {
    double simulation_time_seconds{};
    std::string model_name;
    std::vector<double> params;
};

class InputParser {
public:
    [[nodiscard]] static InputConfig parse_file(const std::string& path);
};
