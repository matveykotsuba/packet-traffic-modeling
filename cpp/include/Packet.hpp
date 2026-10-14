#pragma once

#include <cstddef>

struct Packet {
    double arrival_time_seconds{};
    std::size_t size_bytes{};
};
