#pragma once

#include "Packet.hpp"

#include <string>
#include <vector>

class CsvWriter {
public:
    static void write_packets(const std::string& path, const std::vector<Packet>& packets);
    static void write_metadata(const std::string& path,
                               const std::string& model_name,
                               double simulation_time_seconds,
                               const std::vector<double>& params);
};
