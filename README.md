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

For the first point (dismissal of candidates), we can compare against the union of row/col/box cells (excluding the cell itself of course).
For the second point on the other hand (setting of unique candidates), we have to check against the cells in the row/col/box without throwing them together.

This is not sufficient to solve all valid sudokus, but it mimics the approach a human would choose.

I am intrigued by the idea of supplementing this with brute-force and backtracking, once the constraint based algorithm doesn't produce improvements.
For initial thoughts on the matter you can check the "[Thoughts on brute force/guessing](README.md#Thoughts on brute force/guessing)" section at the end.

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

Can be found in the [`python`](python) directory.

This was originally done to fatten up my portfolio because I was applying to a job that required Python skills.
I already had a fair bit of experience with Python, so the main thing was figuring out how to solve it.
My initially class based approach has been refactored into seperate functions, to enable easy parallelism using pools of the multiprocessing module.


## C++

Can be found in the [`cpp`](cpp) directory.

I just wanted to learn the basics of C++, because I feel, that it's important to have some exposure to a language with low level of abstraction from the machine.
An understanding of C++ is also needed, when one wants to e.g. do graphics programming, or when performance really matters.

# Testing

The Repository contains a folder [`data`](data), which has a list of one million quizzes and the corresponding solutions, that have been taken from [Kaggle](https://www.kaggle.com/datasets/bryanpark/sudoku).


# Thoughts on brute force/guessing

The basic approach should be relying on logical conclusions, and only if that doesn't improve our knowledge about the solution should we make a guess.
After making a guess, we should go back to logical reasoning again, until it doesn't improve matters anymore (after which we guess again).

Guesses should be made to introduce only a minimum of uncertainty, and they need to be reversible.

The advantages of low uncertainty of course being the high certainty of being correct, that follows from it.
A low amount of uncertainty can be ensured, by guessing first numbers for those cells with a minimal amount of candidates.

Even though we prefer guesses with higher certainty, there is a high chance, our guess is wrong (50 % assuming just two candidates).

Therefore, we need to be able to find out, whether a guess made our riddle intractable, and be able to restore the state to the prior.

Intractability can be checked, by determining whether a cell has no valid candidates left.

Unwinding a guess is easy, if you just keep a copy of the previous state around. 
With the array of bitsets approach, this should be cheap. 
Copy init a new array of bitsets from the old state, and keep a reference to that together with the information which number was set by the guess.
We can push this on a stack, and if it turns out, we went wrong somewhere, we pop the guess of the stack, and restore the previous state from the struct.

We also need to keep track of the guesses, that we proved to produce intractable situations.
Problematic is, that we can only really dismiss guesses at the "first" level. A guess that has been made down the line, might be correct, even though it produces an intractable situation, because a previous guess was wrong.
We can however conclude, that the guess is incompatible with the prior guess. 

