#include <set>
#include <sstream>

#include "sudoku.hpp"
#include "utils.hpp"

using namespace std;

const string Sudoku::VALID_SYMBOLS = "0123456789";
const int Sudoku::ADDRESS_RANGE[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

vector<Sudoku> Sudoku::from_file(const string& file_path) {
    vector<string> sudoku_strings = read_sudoku_file(file_path);

    vector<Sudoku> sudokus;
    for (const string& sudoku_string: sudoku_strings)
        sudokus.emplace_back(sudoku_string);

    return sudokus;
}

Sudoku::Sudoku(const string &sudoku_string) {
    for (int i = 0; i < sudoku_string.size(); i++) {
        char symbol = sudoku_string[i];

        // by checking if the symbol is valid, we bounded it in [0-9],
        // and the following operations can be done without problem
        const bool valid = VALID_SYMBOLS.find(symbol) != std::string::npos;
        if (!valid) throw exception();

        // get the int value of the symbol (not ascii!)
        int value = symbol - '0';

        // find out, where we are
        div_t pos = div(i, 9);

        int row, col;
        row = pos.quot;
        col = pos.rem;

        // if we don't know the cell value (indicated by a zero),
        // we set all candidate bits, otherwise only the correct bit
        if (value == 0) data[row][col].set();
        else data[row][col].set(value - 1);
    }
}

std::vector<cell_address> Sudoku::get_unknown_addresses() {
    vector<cell_address> addresses;
    for (cell_address c: get_all_addresses()) if (!is_known(c)) addresses.push_back(c);
    return addresses;
}

std::vector<cell_address> Sudoku::get_all_addresses() {
    vector<cell_address> addresses;
    for (int i: ADDRESS_RANGE) for (int j: ADDRESS_RANGE) addresses.emplace_back(i, j);
    return addresses;
}

std::vector<cell_address> Sudoku::get_row_addresses(int row) {
    vector<cell_address> addresses;
    for (int i: ADDRESS_RANGE) addresses.emplace_back(row, i);
    return addresses;
}

std::vector<cell_address> Sudoku::get_col_addresses(int col) {
    vector<cell_address> addresses;
    for (int i: ADDRESS_RANGE) addresses.emplace_back(i, col);
    return addresses;
}

std::vector<cell_address> Sudoku::get_box_addresses(int row, int col) {
    int row_offset = div(row, 3).quot * 3;
    int rows[3] = {row_offset, row_offset + 1, row_offset + 2};

    int col_offset = div(col, 3).quot * 3;
    int cols[3] = {col_offset, col_offset + 1, col_offset + 2};

    vector<cell_address> addresses;

    for (int row: rows) {
        for (int col: cols) {
            addresses.emplace_back(row, col);
        }
    }

    return addresses;
}

std::vector<cell_address> Sudoku::get_box_addresses(cell_address address) {
    return get_box_addresses(address.row, address.col);
}

std::vector<cell_address> Sudoku::get_codependent_addresses(int row, int col) {
    vector<cell_address> row_addresses = get_row_addresses(row);
    vector<cell_address> col_addresses = get_col_addresses(col);
    vector<cell_address> box_addresses = get_box_addresses(row, col);

    set<cell_address> address_set;

    for (cell_address address: row_addresses) address_set.emplace(address.row, address.col);
    for (cell_address address: col_addresses) address_set.emplace(address.row, address.col);
    for (cell_address address: box_addresses) address_set.emplace(address.row, address.col);

    vector<cell_address> addresses(address_set.begin(), address_set.end());

    return addresses;
}

std::vector<cell_address> Sudoku::get_codependent_addresses(cell_address address) {
    return get_codependent_addresses(address.row, address.col);
}

std::vector<int> Sudoku::get_candidates(int row, int col) {
    vector<int> candidates;

    for (int i=0; i < data[row][col].size(); i++) if (data[row][col].test(i)) candidates.push_back(i+1);

    return candidates;
}

std::vector<int> Sudoku::get_candidates(cell_address address) {
    return get_candidates(address.row, address.col);
}

int Sudoku::get_value(int row, int col) {
    bitset b = data[row][col];
    // if there are no candidates left, or more than one we return 0
    if (b.count() == 0 || b.count() > 1) return 0;

    int val = 0;
    while (b.to_ulong() != 0) {
        b >>= 1;
        val++;
    }

    return val;
}

int Sudoku::get_value(cell_address address) {
    return get_value(address.row, address.col);
}

bool Sudoku::update_candidates(int row, int col) {
    // we are interested in knowing whether any changes were done
    bool updated = false;

    vector<cell_address> codependents = get_codependent_addresses(row, col);
    // if we have a candidate, that can only fit in this cell, it is the correct value
    for (cell_address c: codependents) {
        // if a value is already used in one of the codependent cells
        // it can't be valid in this cell, so we use the inverse of
        // the codependent bitset as an and bitmask to exclude those
        if (is_known(c)) {
            bitset inverse = ~data[c.row][c.col];
            bitset common = data[row][col] & inverse;
            if (common != data[row][col]) {
                data[row][col] = common;
                updated = true;
            }
        }
    }

    if (!is_known(row, col)) {
        vector<tuple<vector<cell_address>, bitset<9>>> influences = {
                make_tuple(get_row_addresses(row), bitset<9>(0)),
                make_tuple(get_col_addresses(col), bitset<9>(0)),
                make_tuple(get_box_addresses(row, col), bitset<9>(0))
        };

        for (tuple<vector<cell_address>, bitset<9>> influence: influences) {
            // get all bits that are set along the same 'axis of influence', e.g. row/col/box
            for (cell_address c: get<0>(influence)) {
                // skip the relevant cell itself
                if (c == cell_address(row, col)) continue;
                get<1>(influence) |= data[c.row][c.col];
            }

            // we check for the bits, that could only possibly be valid here
            // count should in theory never be something else than one or zero,
            // so optimizing with !only_possible_here.none() might be possible?
            // but I'm scared to use it, because bad logic ate my dog and left
            bitset only_possible_here = ~get<1>(influence) & data[row][col];
            if (only_possible_here.count() == 1 && only_possible_here != data[row][col]) {
                // if it's only passible here, we found our value, set it, remember update, and leave the loop
                data[col][row] = only_possible_here;
                updated = true;
                break;
            }
        }
    }

    return updated;
}

bool Sudoku::update_candidates(cell_address address) {
    return update_candidates(address.row, address.col);
}

void Sudoku::solve() {
    vector<cell_address> unknown_cells;
    do {
         unknown_cells = get_unknown_addresses();

        bool updated = false;
        for (cell_address c: unknown_cells) updated |= update_candidates(c);
        // we were not able to do any updates, so we abort
        if (!updated) break;

    } while (unknown_cells.size());
}

std::string Sudoku::to_string() {
    stringbuf buf;
    for (cell_address c: get_all_addresses()) buf.sputc((char) get_value(c) + '0');
    return buf.str();
}

std::string Sudoku::to_pretty_string() {
    return std::string();
}
