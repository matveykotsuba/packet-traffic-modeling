#include "PoissonTrafficModel.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>

using namespace std;

PoissonTrafficModel::PoissonTrafficModel(double mean_delay_seconds,
                                         double mean_packet_size_bytes,
                                         optional<uint32_t> seed)
    : mean_delay_seconds_(mean_delay_seconds),
        mean_packet_size_bytes_(mean_packet_size_bytes),
        rng_(seed.value_or(random_device{}())) {
    if (!(mean_delay_seconds_ > 0.0) || !isfinite(mean_delay_seconds_)) {
        throw invalid_argument("poisson model: mean_delay_seconds must be positive");
    }
    if (!isfinite(1.0 / mean_delay_seconds_)) {
        throw invalid_argument("poisson model: mean_delay_seconds is too small");
    }
    if (!(mean_packet_size_bytes_ > 0.0) || !isfinite(mean_packet_size_bytes_)) {
        throw invalid_argument("poisson model: mean_packet_size_bytes must be positive");
    }
    if (mean_packet_size_bytes_ > static_cast<double>(numeric_limits<size_t>::max())) {
        throw invalid_argument("poisson model: mean_packet_size_bytes is too large");
    }
}

string PoissonTrafficModel::name() const {
    return "poisson";
}

vector<Packet> PoissonTrafficModel::generate(double simulation_time_seconds) {
    vector<Packet> packets;

    if (simulation_time_seconds <= 0.0) {
        return packets;
    }

    exponential_distribution<double> delay_distribution(1.0 / mean_delay_seconds_);
    exponential_distribution<double> size_distribution(1.0 / mean_packet_size_bytes_);

    constexpr size_t max_packets = 10'000'000U;

    double time = delay_distribution(rng_);
    while (time <= simulation_time_seconds) {
        const double raw_size = max(1.0, round(size_distribution(rng_)));
        if (raw_size > static_cast<double>(numeric_limits<size_t>::max())) {
            throw runtime_error("poisson model: generated packet size is too large");
        }
        const auto size = static_cast<size_t>(raw_size);
        packets.push_back(Packet{ time, size });

        if (packets.size() >= max_packets) {
            throw runtime_error("poisson model: too many packets generated; check parameters");
        }

        time += delay_distribution(rng_);
    }

    return packets;
}
