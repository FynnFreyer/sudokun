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

## Performance

On my laptop with an eight core AMD Ryzen 7 PRO 4750U and 32 GB of RAM, I can run a file with 100,000 quizzes in ca. 40 seconds.

Ingesting the data takes approximately 10s, and the computations another 30s.
Ingesting the data could probably be optimized, but that didn't feel like the important bit.

I also ran a benchmark on the one million line strong [`quizzes.sudoku`](../data/quizzes.sudoku) file, with

    python -m timeit -n 1 -r 5 -u sec -s 'from subprocess import run' -- 'run(["./src/sudokun.py", "../data/quizzes.sudoku"])'

and the result was `1 loop, best of 5: 323 sec per loop`.
So a little over five minutes for one million sudokus.
I consider this sufficiently performant for a scripting language.

I could of course change my datastructures, to e.g. use numpy arrays.
That would probably net me a significant performance increase.
But I want to stick to pure Python without external dependencies for now.

## TODOs

- add tests
- garbage collection
  - e.g. del all the dicts once they are not needed anymore
  - or maybe rather just call gc.collect() every x iterations
  - actually because of pythons reference counting gc scheme
    none of this should be needed. better search for circular 
    references, that prevent proper garbage collection instead
- supplement the algorithm with backtracking
- add a proper description of the algorithm/program structure
  - explain the candidate caching
  - maybe also something on the parallelism aspect
