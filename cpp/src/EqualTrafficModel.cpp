#include "EqualTrafficModel.hpp"

#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>

using namespace std;

EqualTrafficModel::EqualTrafficModel(double interval_seconds, double packet_size_bytes)
    : interval_seconds_(interval_seconds) {
    if (!(interval_seconds_ > 0.0) || !isfinite(interval_seconds_)) {
        throw invalid_argument("equal model: interval_seconds must be positive");
    }
    if (!(packet_size_bytes > 0.0) || !isfinite(packet_size_bytes)) {
        throw invalid_argument("equal model: packet_size_bytes must be positive");
    }
    if (packet_size_bytes > static_cast<double>(numeric_limits<size_t>::max())) {
        throw invalid_argument("equal model: packet_size_bytes is too large");
    }
    packet_size_bytes_ = static_cast<size_t>(round(packet_size_bytes));
    if (packet_size_bytes_ == 0U) {
        packet_size_bytes_ = 1U;
    }
}

string EqualTrafficModel::name() const {
    return "equal";
}

vector<Packet> EqualTrafficModel::generate(double simulation_time_seconds) {
    vector<Packet> packets;

    if (simulation_time_seconds <= 0.0) {
        return packets;
    }

    constexpr size_t max_packets = 10'000'000U;

    for (double time = 0.0; time <= simulation_time_seconds + 1e-12; time += interval_seconds_) {
        if (packets.size() >= max_packets) {
            throw runtime_error("equal model: too many packets generated; check interval_seconds");
        }
        packets.push_back(Packet{ time, packet_size_bytes_ });
    }

    return packets;
}
