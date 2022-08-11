#include <iostream>

#include "sudoku.hpp"

using namespace std;

int main() {
    vector<Sudoku> sudokus = Sudoku::from_file("../sudoku.txt");

    for (Sudoku s: sudokus) {
        cout << s.to_string() << endl;
        s.solve();
        cout << s.to_string() << endl;
        cout << endl;

    }

    return 0;
}
