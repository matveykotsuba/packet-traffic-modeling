#pragma once

#include "Packet.hpp"

#include <string>
#include <vector>

class TrafficModel {
public:
    virtual ~TrafficModel() = default;

    [[nodiscard]] virtual std::string name() const = 0;
    [[nodiscard]] virtual std::vector<Packet> generate(double simulation_time_seconds) = 0;
};
