#pragma once

#include "TrafficModel.hpp"

class EqualTrafficModel final : public TrafficModel {
public:
    EqualTrafficModel(double interval_seconds, double packet_size_bytes);

    [[nodiscard]] std::string name() const override;
    [[nodiscard]] std::vector<Packet> generate(double simulation_time_seconds) override;

private:
    double interval_seconds_{};
    std::size_t packet_size_bytes_{};
};
