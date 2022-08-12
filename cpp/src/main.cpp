#include <iostream>
#include <boost/program_options.hpp>

#include "utils.hpp"
#include "sudoku.hpp"

using namespace std;


int main(int argc, char** argv) {

    try {
        po::variables_map var_map = parse_args(argc, argv);
    } catch (int error) {
        return error;
    }
    
    vector<Sudoku> sudokus = Sudoku::from_file("../../data/quizzes.sudoku");

    for (Sudoku s: sudokus) {
        cout << s.to_string() << endl;
        s.solve();
        cout << s.to_string() << endl;
        cout << endl;

    }

    return 0;
}
