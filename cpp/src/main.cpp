#include <iostream>
#include <boost/program_options.hpp>

#include "utils.hpp"
#include "sudoku.hpp"

using namespace std;


int main(int argc, char** argv) {
    po::variables_map var_map;
    try {
        var_map = parse_args(argc, argv);
    } catch (int error) {
        return error;
    }

    vector<Sudoku> sudokus = Sudoku::from_file(var_map["input"].as<vector<string>>()[0]);

    for (Sudoku s: sudokus) {
        s.solve();
        if (var_map.count("pretty")) {
            cout << s.to_pretty_string() << endl;
        } else {
            cout << s.to_string() << endl;
        }

    }

    return 0;
}
