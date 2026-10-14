#include "InputParser.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace std;

namespace {

    string trim(const string& value) {
        auto begin = value.begin();
        while (begin != value.end() && isspace(static_cast<unsigned char>(*begin)) != 0) {
            ++begin;
        }

        auto end = value.end();
        do {
            if (end == begin) {
                return string(begin, end);
            }
            --end;
        } while (isspace(static_cast<unsigned char>(*end)) != 0);

        return string(begin, end + 1);
    }

    string remove_comment(const string& line) {
        const auto hash_position = line.find('#');
        if (hash_position == string::npos) {
            return line;
        }
        return line.substr(0, hash_position);
    }

    string lowercase(string value) {
        transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
            return static_cast<char>(tolower(c));
            });
        return value;
    }

    vector<string> read_meaningful_lines(const string& path) {
        ifstream input(path);
        if (!input.is_open()) {
            throw runtime_error("cannot open input file: " + path);
        }

        vector<string> lines;
        string line;
        while (getline(input, line)) {
            line = trim(remove_comment(line));
            if (!line.empty()) {
                lines.push_back(line);
            }
        }

        return lines;
    }

}

InputConfig InputParser::parse_file(const string& path) {
    const auto lines = read_meaningful_lines(path);
    if (lines.size() != 2U) {
        throw runtime_error("input file must contain exactly two non-empty lines");
    }

    InputConfig config{};

    try {
        size_t processed = 0;
        config.simulation_time_seconds = stod(lines[0], &processed);
        if (processed != lines[0].size()) {
            throw runtime_error("extra characters in simulation time");
        }
    }
    catch (const exception&) {
        throw runtime_error("first line must be simulation time as floating-point number");
    }

    if (!isfinite(config.simulation_time_seconds) ||
        config.simulation_time_seconds < 0.0 ||
        config.simulation_time_seconds > 3600.0) {
        throw runtime_error("simulation time must be in range [0, 3600] seconds");
    }

    string model_line = lines[1];
    replace(model_line.begin(), model_line.end(), ':', ' ');

    istringstream parser(model_line);
    if (!(parser >> config.model_name)) {
        throw runtime_error("second line must contain traffic model name");
    }
    config.model_name = lowercase(config.model_name);

    double param = 0.0;
    while (parser >> param) {
        config.params.push_back(param);
    }

    if (!parser.eof()) {
        throw runtime_error("second line contains non-numeric model parameter");
    }

    return config;
}
