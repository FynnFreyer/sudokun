#! /usr/bin/env python3
from dataclasses import dataclass

class Sudoku:
    def __init__(self, file):
        with open(file, 'r') as f:
            lines = f.read().splitlines()
            assert len(lines) == 9, 'Sudoku has wrong format'
            assert all([len(line) == 9 for line in lines]), 'Sudoku has wrong format'

        self.cells = {}
        for row, line in enumerate(lines):
            for col, char in enumerate(line):
                try:
                    self.cells[(row, col)] = int(char)
                except ValueError:
                    raise ValueError('Sudoku hat ungültigen Inhalt.\nUnbekannte werden mit 0 kodiert.')

    def get_cell(self, row, col):
        return self.cells[(row, col)]

    def set_cell(self, row, col, val):
        self.cells[(row, col)] = val
        return

    def get_row(self, row):
        row_vals = list()
        for j in range(9):
            row_vals.append(self.cells[(row, j)])
        return row_vals

    def get_col(self, col):
        col_vals = list()
        for i in range(9):
            col_vals.append(self.cells[(i, col)])
        return col_vals

    def get_subsquare(self, row, col):
        subsquare = list()
        row_offset = (row // 3) * 3
        col_offset = (col // 3) * 3
        for row in range(row_offset, row_offset + 3):
            for col in range(col_offset, col_offset + 3):
                subsquare.append(self.get_cell(row, col))
        return subsquare

    def get_candidates(self, row, col):
        cell_val = self.get_cell(row, col)
        if cell_val != 0:
            return [cell_val]

        valid_values = [1, 2, 3, 4, 5, 6, 7, 8, 9]

        row_vals = self.get_row(row)
        col_vals = self.get_col(col)
        sub_square_vals = self.get_subsquare(row, col)

        excluded_vals = row_vals + col_vals + sub_square_vals

        candidates = [candidate for candidate in valid_values if candidate not in excluded_vals]

        assert len(candidates) >= 1, 'No possible values left, Sudoku seems unsolvable to me =('

        return candidates

    def try_fill(self, row, col) -> bool:
        candidates = self.get_candidates(row, col)
        if len(candidates) == 1:
            self.set_cell(row, col, candidates[0])
            return True
        return False

    def try_solve(self, iterations=5000):
        for i in range(iterations):
            for row in range(9):
                for col in range(9):
                    self.try_fill(row, col)

    def __str__(self):
        string = ""
        for row in range(9):
            for col in range(9):
                string = string + " " + str(self.get_cell(row, col)) + " |"
            # switch last | of row to newline and add delineation
            string = string[:-1] + "\n"
            if row < 8:
                string = string + "---+---+---+---+---+---+---+---+---\n"
        return string


def main():
    sudoku = Sudoku('sudoku')
    print(sudoku)
    sudoku.try_solve()
    print(sudoku)


if __name__ == '__main__':
    main()

