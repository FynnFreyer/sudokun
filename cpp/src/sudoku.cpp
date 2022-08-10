#include "sudoku.hpp"
#include "utils.hpp"

class Sudoku {
    std::vector<int> candidates[9][9];
    int data[9][9];

    std::vector<int> get_row_values(int row_index);
    std::vector<int> get_col_values(int col_index);
    std::vector<int> get_box_values(int x, int y);

    std::vector<int> get_row_candidates(int row_index);
    std::vector<int> get_col_candidates(int col_index);
    std::vector<int> get_box_candidates(int x, int y);

public:
    Sudoku(std::string file_path);
    void solve();
};
