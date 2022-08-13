# Sudokun

This repository contains some sudoku solvers, implemented in different languages (so far Python and C++).
These are not really good, efficient or otherwise remarkable, but it is a nice problem to familiarise oneself with basic language features.

Also I like Sudokus.

# Approach

I assume that the rules are already understood, and I won't explain them in detail.

The way I determine the solution is based on the following fundamental observations:

- For every unknown cell, there is a list of candidates, that could fit
- Candidate numbers may be dismissed, if they already appear in the same 
    - row
    - column
    - box (3x3 sub box)
- If there is only one candidate for a cell, that must be the correct number
- Even if there are multiple candidates, we can conclude that a candidate is correct, if it's unique among the candidates of the 
    - row
    - column
    - box

This is not sufficient to solve all valid sudokus, but it mimics the approach a human would choose.

I am intrigued by the idea of supplementing this with brute-force and backtracking, once the constraint based algorithm doesn't produce improvements.
An interesting, albeit daunting solution would be Knuths [Algorithm X](https://www.ocf.berkeley.edu/~jchu/publicportal/sudoku/0011047.pdf), and if I ever have 5 years, to understand and implement it, I might do so.

# Input file format

Sudokus are input via a plain text file (c.f. [Testing](README.md#Testing)), whose path is passed as the first positional argument when running the program.
Every line has to have exactly 81 (9 x 9) charcters, one corresponding to each cell.
Ordering is row by row, from the top left to the bottom right.
Known cells get their respective numbers (`1` through `9`), and unknown cells get a zero (`0`).
Empty lines, and lines starting with a hashbang (`#`) are ignored.

# Implementations

This scheme for solving the sudokus is implemented in multiple languages.

Details on the implementations are can be found in the `README.md` files in the respective folders.

## Python

This was originally done to fatten up my portfolio because I was applying to a job that required Python skills.
I already had a fair bit of experience with Python, so the main thing was figuring out how to solve it.
My initially class based approach has been refactored into seperate functions, to enable easy parallelism using pools of the multiprocessing module.

## C++

This is not yet implemented fully.

I just wanted to learn the basics of C++, because I feel, that it's important to have some exposure to a low level of abstraction from the machine.
An understanding of C++ is also needed, when one wants to e.g. do graphics programming, or when performance really matters.
I basically want to port my Python version, which probably makes it less C++y, and more Pythony, than it should be, but everyone has to start somewhere.

# Testing

The Repository contains a folder [`data`](data), which has a list of one million quizzes and the corresponding solutions, that have been taken from [Kaggle](https://www.kaggle.com/datasets/bryanpark/sudoku).
