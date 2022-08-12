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
        int row = pos.quot, col = pos.rem;

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

    for (int row: rows) for (int col: cols) addresses.emplace_back(row, col);

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
    // we are interested in knowing whether any changes were made
    bool updated = false;
    int foo = row;

    // get all cells that influence, the possible values of our cell
    vector<cell_address> codependents = get_codependent_addresses(row, col);
    // if we have a candidate, that can only fit in this cell, it is the correct value
    for (cell_address c: codependents) {
        // skip the current cell itself, would make the differences we're looking for meaningless
        if (c == cell_address(row, col)) continue;
        // if a value is already used in one of the codependent cells it can't be valid
        // therefore we check whether the value of the comparison cell is known
        if (is_known(c)) {
            // the inverse of the codependent bitset leaves all those candidates, that are not excluded yet
            bitset not_excluded = ~data[c.row][c.col];
            // if we use it as a bitmask we can exclude the taken values with a bitwise and
            bitset still_possible = data[row][col] & not_excluded;
            // if there is a difference between what we have in our candidates, and what's possible
            if (data[row][col] != still_possible) {
                // we update accordingly and remember, that we did an update
                data[row][col] &= not_excluded;
                updated = true;
            }
        }
    }


    // if we did not yet find a solution for the current cell, then we try a different strategy
    if (!is_known(row, col)) {
        // we have to check for the contents along the same 'axis of influence', e.g. row/col/box seperately!
        vector<vector<cell_address>> influences = {
                get_row_addresses(row),
                get_col_addresses(col),
                get_box_addresses(row, col)
        };

        for (vector<cell_address> influence: influences) {
            // get all bits that are set in the same row/col/box,
            // e.g. all possible candidates
            bitset<9> candidates;
            for (cell_address c: influence) {
                // skip the current cell itself, would make the differences we're looking for meaningless
                if (c == cell_address(row, col)) continue;
                candidates |= data[c.row][c.col];
            }

            // we check for the bits, that could only possibly be valid here
            // count should in theory never be something else than one or zero
            bitset only_possible_here = ~candidates & data[row][col];
            // if we found a value that can only fit here
            if (only_possible_here.count() == 1) {
                // we found our value, so we set it, remember we did an update, and leave the loop
                data[row][col] &= only_possible_here;
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
        for (cell_address c: unknown_cells)
            updated |= update_candidates(c);
        // we were not able to do any further updates, so we abort
        if (!updated) break;

    } while (unknown_cells.size());
}

std::string Sudoku::to_string() {
    stringbuf buf;
    for (cell_address c: get_all_addresses()) buf.sputc((char) get_value(c) + '0');
    return buf.str();
}

std::string Sudoku::to_pretty_string() {
    int range[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

    string pretty_string = "";

    string row_delineator_thin = "\n---+---+---++---+---+---++---+---+---\n";
    string row_delineator_thick = "\n===+===+===++===+===+===++===+===+===\n";

    string col_delineator_thin = "|";
    string col_delineator_thick = "||";

    for (int row: range) {
        for (int col: range) {
            bool last_col = col == 8;
            bool col_divisible_by_three = (col + 1) % 3 == 0;

            string col_delineator = last_col ? "" : col_divisible_by_three ? col_delineator_thick : col_delineator_thin;

            pretty_string += " " + std::to_string(get_value(row, col)) + " " + col_delineator;
        }
        bool last_row = row == 8;
        bool row_divisible_by_three = (row + 1) % 3 == 0;

        string row_delineator = last_row ? "" : row_divisible_by_three ? row_delineator_thick : row_delineator_thin;

        pretty_string += row_delineator;
    }
    return pretty_string + '\n';
}
