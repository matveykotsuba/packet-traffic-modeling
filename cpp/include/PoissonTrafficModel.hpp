#pragma once

#include "TrafficModel.hpp"

#include <cstdint>
#include <optional>
#include <random>

class PoissonTrafficModel final : public TrafficModel {
public:
    PoissonTrafficModel(double mean_delay_seconds,
                        double mean_packet_size_bytes,
                        std::optional<std::uint32_t> seed = std::nullopt);

    [[nodiscard]] std::string name() const override;
    [[nodiscard]] std::vector<Packet> generate(double simulation_time_seconds) override;

private:
    double mean_delay_seconds_{};
    double mean_packet_size_bytes_{};
    std::mt19937 rng_;
};
