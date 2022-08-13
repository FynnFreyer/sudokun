# C++

The data is represented internally as a 9x9 matrix of length 9 bitsets.
Every cell in the matrix corresponds to a cell in the sudoku, and every bit in the bitset corresponds to a number being a possible candidate, when set, or not, when not set.


we read the sudoku file into a vector of work_unit 
we pass a reference to that vector to our threads


# TODOs

- extract multithreading logic (do_work and crunch)
  - either to seperate module
  - or to utils (difficult if sudoku needs to include utils,   
    but does it really after deleting from_file?)
- make pretty_print static and have it take a string.
- documentation 
  - inline
  - docstrings (doxygen?)
- finish up readme
  - approach
  - build
  - usage
  - performance
- implement tests

