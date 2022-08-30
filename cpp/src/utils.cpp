#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <thread>
#include <mutex>

#include "utils.hpp"
#include "sudoku.hpp"

using namespace std;


vector<string> read_sudoku_file(const string& file) {
    vector<string> sudokus;

    ifstream input_file_stream(file);
    if (input_file_stream.is_open()) {
        string line;
        while (getline(input_file_stream, line)) {
            // filter empty lines and comments
            if (!(line.empty() || line.starts_with('#'))) {
                // add it to the sudokus vector
                sudokus.push_back(line);
            }
        }
        input_file_stream.close();
    } else cout << "Unable to open file " << file;

    return sudokus;
}

// our mutex to control read (and pop) access to the work_unit vector
mutex read_mtx;

void do_work(vector<string>& sudokus, string solved[]) {
    // we need to initialize the unit before going into the critical section, or it goes out of scope
    string sudoku_string;
    int i = -1;

    do {
        { // critical section, reading from the vector
            lock_guard<mutex> lock(read_mtx);
            if (sudokus.empty()) break;
            i = sudokus.size() - 1;
            sudoku_string = sudokus.back();
            sudokus.pop_back();
        }

        // read sudoku string into sudoku and solve it
        Sudoku sudoku(sudoku_string);
        sudoku.solve();

        // write the solution to the array of solved sudokus
        solved[i] = sudoku.to_string();

    } while (true);
}

vector<string> crunch(vector<string>& sudokus) {
    // because the threads pop from the vector, we need to remember the size at the start
    int sudokus_total = sudokus.size();
    // get an array of strings, where the threads can write out the solutions
    auto* solved = new string[sudokus_total];

    // get the number of cores
    unsigned int cpus = thread::hardware_concurrency();
    if (cpus == 0) cpus++;  // seems like no concurrency supported, so only one thread
    const unsigned int thread_count = cpus;  // make it a const, probably unnecessary

    // our thread pool
    vector<thread> threads;
    for (int i = 0; i < thread_count; i++) {
        // instantiate our threads
        threads.emplace_back(do_work, ref(sudokus), ref(solved));
    }

    // join the threads
    for (int i = 0; i < thread_count; i++) {
        if (threads[i].joinable()) {
            threads[i].join();
        }
    }

    // take the solved sudoku array, and turn it into a vector (fewer issues with pointer decay, and generally easier handling)
    vector<string> solved_vector = vector(solved, solved + sudokus_total);
    return solved_vector;
}
