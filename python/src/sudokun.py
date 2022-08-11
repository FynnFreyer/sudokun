#! /usr/bin/env python3

import argparse
from multiprocessing import Pool


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('sudoku_file', type=parse_sudoku_file, help='the input file, containing sudokus to solve.')
    parser.add_argument('-p', '--pretty', action='store_true', help='pretty print the solved sudokus.')
    return parser.parse_args()


def parse_sudoku_file(path: str):
    with open(path, 'r') as f:
        indexed_lines = [(i, line.strip()) for i, line in enumerate(f.readlines())
                         if not (line == "\n" or line.startswith('#'))]

    sudokus = []
    for i, line in indexed_lines:
        assert len(line) == 81, f'Sudoku has wrong format: line {i} has length {len(line)}, needs to be 81'
        invalid_symbols = [char for char in line if char not in '.0123456789']
        assert not invalid_symbols, f'Sudoku has wrong format: line {i} has ' \
                                    f'unrecognizable char{"s" if len(invalid_symbols) > 1 else ""}: ' \
                                    f'{", ".join(invalid_symbols)}'

        sudokus.append(from_string(line))

    return sudokus


def from_string(sudoku_string: str) -> dict[tuple[int, int], int]:
    """
    Parse a string representation of a sudoku, and return it as a dict[tuple[int, int], int].

    :param sudoku_string: The string representing the sudoku.
    :return: The parsed sudoku.
    """
    cells = {}
    for j, char in enumerate(sudoku_string):
        row, col = divmod(j, 9)
        char: str = char.replace('.', '0')
        cells[(row, col)] = int(char)
    return cells


def to_string(sudoku: dict[tuple[int, int], int]) -> str:
    """
    Get a string representation of a sudoku.

    :param sudoku: The sudoku to format.
    :return: A string representation of the sudoku.
    """
    symbols = []
    for i in range(81):
        x, y = divmod(i, 9)
        cell_value = sudoku[x, y]
        symbols.append(str(cell_value) if cell_value != 0 else '.')
    return ''.join(symbols)


def to_pretty_string(sudoku: dict[tuple[int, int], int]) -> str:
    """
    Get a pretty string representation of a sudoku.

    :param sudoku: The sudoku to format.
    :return: A pretty string representation of the sudoku.
    """
    pretty_string = ''

    row_delineator_thin = "\n---+---+---++---+---+---++---+---+---\n"
    row_delineator_thick = "\n===+===+===++===+===+===++===+===+===\n"

    col_delineator_thin = '|'
    col_delineator_thick = '||'

    for row in range(9):
        for col in range(9):
            last_col = col == 8
            col_divisible_by_three = (col + 1) % 3 == 0

            col_delineator = '' if last_col else col_delineator_thin if not col_divisible_by_three else col_delineator_thick

            pretty_string += f' {sudoku[row, col]} ' + col_delineator

        last_row = row == 8
        row_divisible_by_three = (row + 1) % 3 == 0

        row_delineator = '' if last_row else row_delineator_thin if not row_divisible_by_three else row_delineator_thick

        pretty_string += row_delineator

    return pretty_string + '\n'


def get_row_address_tuples(row: int) -> list[tuple[int, int]]:
    """
    Get a list of address tuples, that correspond to the cells in the same row as the passed row index.

    :param row: Index of the row.
    :return: List of address tuples in the same box.
    """
    return [(row, col) for col in range(9)]


def get_col_address_tuples(col: int) -> list[tuple[int, int]]:
    """
    Get a list of address tuples, that correspond to the cells in the same column as the passed culumn index.

    :param col: Index of the column.
    :return: List of address tuples in the same box.
    """
    return [(row, col) for row in range(9)]


def get_box_address_tuples(row: int, col: int) -> list[tuple[int, int]]:
    """
    Get a list of address tuples, that correspond to the cells in the same box as the passed cell.

    :param row: Index of the row.
    :param col: Index of the column.
    :return: List of address tuples in the same box.
    """
    row_offset = (row // 3) * 3
    rows = list(range(row_offset, row_offset + 3))

    col_offset = (col // 3) * 3
    cols = list(range(col_offset, col_offset + 3))

    return [(row, col) for row in rows for col in cols]


def get_codependent_address_tuples(row: int, col: int) -> list[tuple[int, int]]:
    """
    Get a list of address tuples, that correspond to the cells either in the same row, colum, or box as the passed cell.

    These are the cells, whose values influence potential candidates of the passed cell,
    and whose candidates in turn are influenced by the value of the passed cell, thus we call them codependent.

    :param row: Index of the row.
    :param col: Index of the column.
    :return: List of address tuples in the same row, colum, or box.
    """

    addresses = get_row_address_tuples(row) + get_col_address_tuples(col) + get_box_address_tuples(row, col)
    return list(set([address for address in addresses]))


def get_candidates(sudoku: dict[tuple[int, int], int], row: int, col: int) -> set[int]:
    """
    Get a set of candidate values for the passed cell.

    :param sudoku: The sudoku.
    :param row: Index of the row.
    :param col: Index of the column.
    :return: Set of candidate values for the cell.
    """

    if sudoku[row, col] != 0:
        return {sudoku[row, col]}

    potential_vals = {1, 2, 3, 4, 5, 6, 7, 8, 9}
    excluded_vals = set([sudoku[address] for address in (get_codependent_address_tuples(row, col))])

    candidates = potential_vals - excluded_vals

    # # TODO maybe we shouldn't throw on unsolvable sudokus, but handle them gracefully instead, cf. solve
    # assert len(candidates) >= 1, f'No possible values left, this sudoku seems unsolvable to me =(\n{sudoku}'

    return candidates


def solve(sudoku: dict[tuple[int, int], int]) -> dict[tuple[int, int], int]:
    """
    Solve the passed sudoku. Or try to, anyway.

    This is NOT a brute force search through the space of possible solutions,
    but a constraint based filling in of cells whose values can be deduced.

    If no more values can be deduced, but the sudoku is still unsolved, we just return it.
    In future, the constraint based approach might be supplemented with a brute force method, or selective guessing.

    :param sudoku: The sudoku to solve.
    :return: The (partially) solved sudoku.
    """
    all_cells = [(row, col) for row in range(9) for col in range(9)]
    # mapping from cell address to its candidate values
    candidate_map = {cell: get_candidates(sudoku, *cell) for cell in all_cells}

    # we compute all initial candidates once in the beginning
    # afterwards only do updates for unknown cells

    # while there are unsolved cells left:
    while unsolved_cells := [cell for cell in all_cells if sudoku[cell] == 0]:
        # if we go through all unsolved cells, but find nothing to update, we can't solve this sudoku
        # for this reason we remember, when we do an update, to check afterwards if changes happened
        updated = False
        # iterate through all unsolved cells
        for cell in unsolved_cells:
            # look up the candidate set for this cell
            cell_candidates = candidate_map[cell]

            # we have not found the correct number yet
            solution = None

            # if there is only one candidate then it must be the solution
            if len(cell_candidates) == 1:
                solution = list(cell_candidates)[0]

            # we only need to look at it from another angle, if we haven't already found the solution
            if solution is None:
                # if a candidate does not fit any other cell of the same row/col/box, then it is the solution
                # we can not check against the union of these candidate sets!

                # compute the candidates for the row/col/box excluding the cell itself
                other_row_addresses = [address for address in get_row_address_tuples(cell[0]) if address != cell]
                other_row_candidates = set()
                other_row_candidates.update(*[candidate_map[address] for address in other_row_addresses])

                other_col_addresses = [address for address in get_col_address_tuples(cell[1]) if address != cell]
                other_col_candidates = set()
                other_col_candidates.update(*[candidate_map[address] for address in other_col_addresses])

                other_box_addresses = [address for address in get_box_address_tuples(*cell) if address != cell]
                other_box_candidates = set()
                other_box_candidates.update(*[candidate_map[address] for address in other_box_addresses])

                for candidate in cell_candidates:
                    if ((candidate not in other_row_candidates)
                            or (candidate not in other_col_candidates)
                            or (candidate not in other_box_candidates)):
                        solution = candidate
                        break  # don't check the other candidates

            # if we did find a solution
            if solution is not None:
                # set it as the cell value
                sudoku[cell] = solution

                # determine addresses of all cells, that influence this cell, and vice versa are influenced by it
                # e.g. all cells in the same row/col/box, excluding this cell itself and look up their candidates
                codependents = get_codependent_address_tuples(*cell)
                codependent_candidates = set()
                codependent_candidates.update(*[candidate_map[codependent] for codependent in codependents])

                # compute the changes for the affected cells in the candidate_map
                candidate_map[cell] = {solution}
                codependent_diffs = {codependent: candidate_map[codependent] - {solution} for codependent in
                                     codependents}
                candidate_map.update(codependent_diffs)

                # remember that we did an update during this iteration
                updated = True

        # # TODO maybe we shouldn't throw on unsolvable sudokus, but handle them gracefully instead, cf. get_cell_candidates
        # assert updated, f'No new cells found in last iteration, this sudoku seems unsolvable to me =(\n{sudoku}'

        if not updated:
            break

    return sudoku


def crunch(sudokus: list[dict[tuple[int, int], int]]) -> list[dict[tuple[int, int], int]]:
    with Pool() as p:
        solved_sudokus = p.map(solve, sudokus)
    return solved_sudokus


def main():
    args = parse_args()
    sudokus = args.sudoku_file

    solved_sudokus = crunch(sudokus)

    for sudoku in solved_sudokus:
        if args.pretty:
            print(to_pretty_string(sudoku))
        else:
            print(to_string(sudoku))

    exit(0)


if __name__ == '__main__':
    main()
