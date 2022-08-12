#include <iostream>
#include <boost/program_options.hpp>

#include "utils.hpp"
#include "sudoku.hpp"

using namespace std;


int main(int argc, char** argv) {

    try {
        po::variables_map var_map = parse_args(argc, argv);

        vector<Sudoku> sudokus = Sudoku::from_file("../../data/quizzes.sudoku");

        for (Sudoku s: sudokus) {
            s.solve();
            if (var_map.count("pretty")) {
                cout << s.to_pretty_string() << endl;
            } else {
                cout << s.to_string() << endl;
            }

        }

    } catch (int error) {
        return error;
    }

    return 0;
}
