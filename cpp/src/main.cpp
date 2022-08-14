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
            auto su = Sudoku(s);
            cout << su.to_pretty_string() << endl;
        } else {
            cout << s << endl;
        }
    }

//    // single threaded
//    for (work_unit unit: units) {
//        Sudoku sudoku(unit.sudoku);
//        sudoku.solve();
//        cout << sudoku.to_string() << endl;
//    }

    return 0;
}
