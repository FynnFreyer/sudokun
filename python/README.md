# Python

Definitely one of my favorite languages, makes programming a breeze.

## Usage

Pass a file, that contains sudokus formatted in accordance with the [specification in the main README](../README.md#Input file format).
Get additional help, and display usage information with `sudokun.py -h`

```
usage: sudokun.py [-h] [-p] sudoku_file

positional arguments:
  sudoku_file   the input file, containing sudokus to solve.

options:
  -h, --help    show this help message and exit
  -p, --pretty  pretty print the solved sudokus.
```

Please spare yourself doing the pretty print on a file with thousands or millions of lines.

## TODOs

- add tests
- garbage collection 
  - e.g. del all the dicts once they are not needed anymore
  - or maybe rather just call gc.collect() every x iterations
- supplement the algorithm with backtracking
