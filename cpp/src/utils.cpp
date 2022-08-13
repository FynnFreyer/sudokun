#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <boost/program_options.hpp>


#include "utils.hpp"

using namespace std;

void print_usage(char* name, const po::options_description& opts_desc) {
    cout << "Usage: " << name << " [options] <input>\n\n";
    cout << opts_desc << "\n";
}

po::variables_map parse_args(int argc, char** argv) {

    po::options_description opts_desc("Allowed options");

    po::options_description opts_desc_generic("Program options");
    opts_desc_generic.add_options()
            ("help,h", "print help message and exit")
            ("pretty,p", "pretty print the output");

    po::options_description opts_desc_hidden("Hidden options");
    opts_desc_hidden.add_options()
            ("input", po::value<vector<string>>(), "input file");

    opts_desc.add(opts_desc_generic).add(opts_desc_hidden);

    po::positional_options_description pos_opts_desc;
    pos_opts_desc.add("input", -1);

    po::variables_map var_map;
    po::store(po::command_line_parser(argc, argv).options(opts_desc).positional(pos_opts_desc).run(), var_map);
    po::notify(var_map);

    if (var_map.count("help")) {
        print_usage(argv[0], opts_desc_generic);
        throw 1;
    }

    if (!var_map.count("input")) {
        cout << "No input file passed!\n" << endl;
        print_usage(argv[0], opts_desc_generic);
        throw 2;
    }

    vector<string> input_vector = var_map["input"].as<vector<string>>();
    if (input_vector.size() > 1) {
        cout << "Only one input file at a time is supported!\n" << endl;
        print_usage(argv[0], opts_desc_generic);
        throw 3;
    }

    namespace fs = filesystem;
    string file_path_string = input_vector[0];
    fs::path file_path(file_path_string);
    if (!fs::exists(file_path)) {
       cout << "The file " << file_path_string << " does not exist, or cannot be read!\n" << endl;
        print_usage(argv[0], opts_desc_generic);
        throw 4;
    }

    return var_map;
}

vector<string> split(string s, string sep, bool keep_sep) {
    // adapted from https://stackoverflow.com/a/14266139
    vector<string> split_list;

    size_t pos = 0;
    string token;
    while ((pos = s.find(sep)) != string::npos) {
        size_t eot = keep_sep ? pos + sep.length() : pos;  // end of token

        token = s.substr(0, eot);
        split_list.push_back(token);

        s.erase(0, pos + sep.length());
    }
    split_list.push_back(s);

    return split_list;
}


vector<work_unit> read_sudoku_file(const string& file) {
    vector<work_unit> units;
    string line;
    ifstream input_file_stream(file);
    if (input_file_stream.is_open()) {
        int i = 0;
        while (getline(input_file_stream, line)) {
            // filter empty lines and comments
            if (!(line.empty() || line.starts_with('#'))) {
                work_unit unit = work_unit(i++, line);
                units.push_back(unit);
            }
        }
        input_file_stream.close();
    }
    else cout << "Unable to open file " << file;

    return units;
}
