#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <boost/program_options.hpp>

#include "utils.hpp"

using namespace std;

po::variables_map parse_args(int argc, char** argv) {

    po::options_description opts_desc("Program options");
    opts_desc.add_options()
            ("help,h", "print help message and exit")
            ("pretty,p", "pretty print the output");

    po::positional_options_description pos_opts_desc;
    pos_opts_desc.add("input", 1);

    po::variables_map var_map;
    po::store(po::command_line_parser(argc, argv).options(opts_desc).positional(pos_opts_desc).run(), var_map);
    po::notify(var_map);

    if (var_map.count("help")) {
        cout << "Usage: " << argv[0] << " [options] <input>\n\n";
        cout << opts_desc << "\n";
        throw 1;
    }

    return var_map;
}


vector<string> split(string s, string sep, bool keep_sep) {
    // adapted from https://stackoverflow.com/a/14266139
    vector<string> split_list;

    size_t pos = 0;
    std::string token;
    while ((pos = s.find(sep)) != std::string::npos) {
        size_t eot = keep_sep ? pos + sep.length() : pos;  // end of token

        token = s.substr(0, eot);
        split_list.push_back(token);

        s.erase(0, pos + sep.length());
    }
    split_list.push_back(s);

    return split_list;
}

std::vector<std::string> read_sudoku_file(const std::string& file) {
    vector<string> lines;
    string line;
    ifstream input_file_stream(file);
    if (input_file_stream.is_open()) {
        while (getline(input_file_stream, line)) {
            // filter empty lines and comments
            if (line != "" && line[0] != '#') {
                lines.push_back(line);
            }
        }
        input_file_stream.close();
    }
    else cout << "Unable to open file " << file;

    return lines;
}

