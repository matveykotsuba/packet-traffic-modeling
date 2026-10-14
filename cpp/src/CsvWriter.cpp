#include "CsvWriter.hpp"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <stdexcept>

using namespace std;

namespace {

    void ensure_parent_directory_exists(const string& path) {
        const filesystem::path output_path(path);
        const auto parent = output_path.parent_path();
        if (!parent.empty()) {
            filesystem::create_directories(parent);
        }
    }

}

void CsvWriter::write_packets(const string& path, const vector<Packet>& packets) {
    ensure_parent_directory_exists(path);

    ofstream output(path);
    if (!output.is_open()) {
        throw runtime_error("cannot open output CSV file: " + path);
    }

    output << "time,size\n";
    output << fixed << setprecision(6);
    for (const auto& packet : packets) {
        output << packet.arrival_time_seconds << ',' << packet.size_bytes << '\n';
    }
}

void CsvWriter::write_metadata(const string& path,
    const string& model_name,
    double simulation_time_seconds,
    const vector<double>& params) {
    ensure_parent_directory_exists(path);

    ofstream output(path);
    if (!output.is_open()) {
        throw runtime_error("cannot open metadata file: " + path);
    }

    output << "model=" << model_name << '\n';
    output << "simulation_time_seconds=" << fixed << setprecision(6)
        << simulation_time_seconds << '\n';
    output << "params=";
    for (size_t i = 0; i < params.size(); ++i) {
        if (i != 0U) {
            output << ',';
        }
        output << params[i];
    }
    output << '\n';
}
