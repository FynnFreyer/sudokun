# C++

I had not much prior experience wit C++ so far, but I'm blown away by the performance.
Using it was not too bad, even though I don't like all the overhead that compiling, linking and `cmake` introduce.


## Approach

The data is represented internally as a 9x9 matrix of length 9 bitsets.
Every cell in the matrix corresponds to a cell in the sudoku.
Every bit in a bitset corresponds to a number, that is a possible candidate, when set, or not, when not set.

we read the sudoku file into a vector of work_unit
we pass a reference to that vector to our threads


## Building

This program depends on the [boost library](https://www.boost.org/), specifically the `program_options` component.
It's used to parse command line options and generate the help text, and I can't be bothered to roll my own version.
Install boost with `sudo apt install boost-dev` on Debian derived systems and `sudo dnf install boost-devel` on RHEL derived systems.
They provide links to binary downloads for Windows on their website I think.

You can build the project from within the [`cpp`](.) directory with the following commands:

    mkdir build; cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    cmake --build .


## Usage

Pass a file, that contains sudokus formatted in accordance with the [specification in the main README](../README.md#Input file format).
Get additional help, and display usage information with `sudokun -h`

    Usage: ./sudokun [options] <input>
    
    Program options:
      -h [ --help ]         print help message and exit
      -p [ --pretty ]       pretty print the output


## Performance

On my laptop with an eight core AMD Ryzen 7 PRO 4750U and 32 GB of RAM,
I can run [`quizzes.sudoku`](../data/quizzes.sudoku) (one million lines) in ca. 30 seconds.

Testing performance with

    python -m timeit -n 1 -r 5 -u sec -s 'from subprocess import run' -- 'run(["./src/sudokun.py", "../data/quizzes.sudoku"])'

results in `1 loop, best of 5: 30.9 sec per loop`.

That is sooo much better than the Python version.
This might be obvious, because interpreted vs. compiled and all that.
But it's still so embarrassing, that I'm considering reimplementing the Python version with Numpy or something.


# TODOs

- clean up some unused functions
    - get_candidates?
        - maybe make it inline, change return type to bitset,  
          and replace all data[row][col] calls with that for readability
    - some of the get_xxx_addresses funcs
        - codependents?
- documentation
    - inline
    - docstrings (doxygen?)
- finish up readme
    - approach
- implement tests

