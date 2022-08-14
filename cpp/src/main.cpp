#include <iostream>
#include <boost/program_options.hpp>

#include "sudoku.hpp"
#include "utils.hpp"

using namespace std;


int main(int argc, char** argv) {
    po::variables_map var_map;
    try {
        var_map = parse_args(argc, argv);
    } catch (int error) {
        return error;
    }

    string file_path = var_map["input"].as<vector<string>>()[0];

    vector<work_unit> units = read_sudoku_file(file_path);
    vector<string> solved = crunch(units);

    for (const string& s: solved) {
        if (var_map.count("pretty")) {
            cout << Sudoku::to_pretty_string(s) << endl;
        } else {
            cout << s << endl;
        }
    }

    return 0;
}
