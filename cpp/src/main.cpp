#include <iostream>
#include <boost/program_options.hpp>

#include "utils.hpp"
#include "sudoku.hpp"

using namespace std;


#include <thread>
#include <mutex>

mutex read_mtx;

void do_work(vector<work_unit>& units, string solved[]) {
    // we need to initialize the unit before going into the critical section, or it goes out of scope
    work_unit unit = work_unit(-1, "");

    do {
        { // critical section, reading from the vector
            lock_guard<mutex> lock(read_mtx);
            if (units.empty()) break;
            unit = units.back();
            units.pop_back();
        }

        Sudoku sudoku(unit.sudoku);
        sudoku.solve();
        string solution = sudoku.to_string();
        solved[unit.index] = solution;

    } while (true);
}

vector<string> crunch(vector<work_unit>& units) {
    int unit_total = units.size();
    auto* solved = new string[unit_total];

    unsigned int cpus = thread::hardware_concurrency();
    if (cpus == 0) cpus++;  // seems like no concurrency supported, so only one thread
    const unsigned int thread_count = cpus;

    vector<thread> threads;
    for (int i = 0; i < thread_count; i++) {
        threads.emplace_back(do_work, ref(units), ref(solved));
    }

    for (int i = 0; i < thread_count; i++) {
        if (threads[i].joinable()) {
            threads[i].join();
        }
    }

    vector<string> solved_vector = vector(solved, solved + unit_total);

    return solved_vector;
}


int main(int argc, char** argv) {
    po::variables_map var_map;
    try {
        var_map = parse_args(argc, argv);
    } catch (int error) {
        return error;
    }

    string file_path = var_map["input"].as<vector<string>>()[0];

    vector<work_unit> units = read_sudoku_file(file_path);
    vector<string> solved = crunch(units);

    for (const string& s: solved) {
        if (var_map.count("pretty")) {
            auto su = Sudoku(s);
            cout << su.to_pretty_string() << endl;
        } else {
            cout << s << endl;
        }
    }

//    // single threaded
//    for (work_unit unit: units) {
//        Sudoku sudoku(unit.sudoku);
//        sudoku.solve();
//        cout << sudoku.to_string() << endl;
//    }

    return 0;
}
