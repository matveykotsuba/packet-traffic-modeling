#include "CsvWriter.hpp"
#include "EqualTrafficModel.hpp"
#include "InputParser.hpp"
#include "PoissonTrafficModel.hpp"
#include "TrafficModel.hpp"

#include <cmath>
#include <cstdint>
#include <exception>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

namespace {

unique_ptr<TrafficModel> create_model(const InputConfig& config) {
    if (config.model_name == "equal") {
        if (config.params.size() != 2U) {
            throw runtime_error("equal model expects 2 parameters: <interval_seconds> <packet_size_bytes>");
        }
        return make_unique<EqualTrafficModel>(config.params[0], config.params[1]);
    }

    if (config.model_name == "poisson") {
        if (config.params.size() != 2U && config.params.size() != 3U) {
            throw runtime_error("poisson model expects 2 or 3 parameters: <mean_delay_seconds> <mean_packet_size_bytes> [seed]");
        }

        optional<uint32_t> seed;
        if (config.params.size() == 3U) {
            const double seed_value = config.params[2];
            if (!isfinite(seed_value) || seed_value < 0.0 ||
                seed_value > static_cast<double>(numeric_limits<uint32_t>::max()) ||
                floor(seed_value) != seed_value) {
                throw runtime_error("poisson seed must be a non-negative integer");
            }
            seed = static_cast<uint32_t>(seed_value);
        }

        return make_unique<PoissonTrafficModel>(config.params[0], config.params[1], seed);
    }

    throw runtime_error("unknown traffic model: " + config.model_name);
}

void print_usage(const char* executable_name) {
    cerr << "Usage:\n"
         << "  " << executable_name << " <input_file> [output_csv] [metadata_file]\n\n"
         << "Examples:\n"
         << "  " << executable_name << " cpp/examples/equal.txt\n"
         << "  " << executable_name << " cpp/examples/poisson.txt output/traffic.csv output/metadata.txt\n";
}

}

int main(int argc, char* argv[]) {
    try {
        if (argc < 2 || argc > 4) {
            print_usage(argv[0]);
            return 1;
        }

        const string input_file = argv[1];
        const string output_csv = argc >= 3 ? argv[2] : "output/traffic.csv";
        const string metadata_file = argc >= 4 ? argv[3] : "output/metadata.txt";

        const InputConfig config = InputParser::parse_file(input_file);
        auto model = create_model(config);
        const auto packets = model->generate(config.simulation_time_seconds);

        CsvWriter::write_packets(output_csv, packets);
        CsvWriter::write_metadata(metadata_file, model->name(), config.simulation_time_seconds, config.params);

        cout << "Traffic model: " << model->name() << '\n';
        cout << "Simulation time: " << config.simulation_time_seconds << " s\n";
        cout << "Generated packets: " << packets.size() << '\n';
        cout << "CSV saved to: " << output_csv << '\n';
        cout << "Metadata saved to: " << metadata_file << '\n';

        return 0;
    } catch (const exception& error) {
        cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
