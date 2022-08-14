#ifndef SUDOKU_HPP
#define SUDOKU_HPP

#include <vector>
#include <string>
#include <bitset>
#include <tuple>

struct cell_address {
    cell_address(unsigned char row, unsigned char col) : row(row), col(col)  {}

    unsigned char row;
    unsigned char col;

    friend inline bool operator <  (const cell_address& lhs, const cell_address& rhs) {
        return std::tie(lhs.row, lhs.col) < std::tie(rhs.row, rhs.col);
    }

    friend inline bool operator >  (const cell_address& lhs, const cell_address& rhs) { return rhs < lhs; }
    friend inline bool operator <= (const cell_address& lhs, const cell_address& rhs) { return !(lhs > rhs); }
    friend inline bool operator >= (const cell_address& lhs, const cell_address& rhs) { return !(lhs < rhs); }

    friend inline bool operator == (const cell_address& lhs, const cell_address& rhs) {
        return std::tie(lhs.row, lhs.col) == std::tie(rhs.row, rhs.col);
    }

    friend inline bool operator != (const cell_address& lhs, const cell_address& rhs) { return !(lhs == rhs); }

};

class Sudoku {
    static const std::string VALID_SYMBOLS;
    static const int ADDRESS_RANGE[];
    std::bitset<9> data[9][9];

    std::vector<cell_address> get_unknown_addresses();

    static std::vector<cell_address> get_all_addresses();
    static std::vector<cell_address> get_row_addresses(int row);
    static std::vector<cell_address> get_col_addresses(int col);
    static std::vector<cell_address> get_box_addresses(int row, int col);
    static std::vector<cell_address> get_box_addresses(cell_address address);
    static std::vector<cell_address> get_codependent_addresses(int row, int col);
    static std::vector<cell_address> get_codependent_addresses(cell_address address);

    std::vector<int> get_candidates(int row, int col);

    [[maybe_unused]] std::vector<int> get_candidates(cell_address address);

    bool update_candidates(int row, int col);
    bool update_candidates(cell_address address);

    int get_value(int row, int col);
    int get_value(cell_address address);

    bool is_known(int row, int col) {return data[row][col].count() == 1;}
    bool is_known(cell_address address) {return this->is_known(address.row, address.col);}

public:
    explicit Sudoku(const std::string& sudoku_string);
    void solve();

    std::string to_string();
    static std::string to_pretty_string(std::string sudoku);
};

#endif // SUDOKU_HPP
