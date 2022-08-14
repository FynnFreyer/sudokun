#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <thread>
#include <mutex>



#include <boost/program_options.hpp>

#include "utils.hpp"
#include "sudoku.hpp"

using namespace std;

void print_usage(char* name, const po::options_description& opts_desc) {
    cout << "Usage: " << name << " [options] <input>\n\n";
    cout << opts_desc << "\n";
}

po::variables_map parse_args(int argc, char** argv) {

    po::options_description opts_desc("Allowed options");

    po::options_description opts_desc_generic("Program options");
    opts_desc_generic.add_options()
            ("help,h", "print help message and exit")
            ("pretty,p", "pretty print the output");

    po::options_description opts_desc_hidden("Hidden options");
    opts_desc_hidden.add_options()
            ("input", po::value<vector<string>>(), "input file");

    opts_desc.add(opts_desc_generic).add(opts_desc_hidden);

    po::positional_options_description pos_opts_desc;
    pos_opts_desc.add("input", -1);

    po::variables_map var_map;
    po::store(po::command_line_parser(argc, argv).options(opts_desc).positional(pos_opts_desc).run(), var_map);
    po::notify(var_map);

    if (var_map.count("help")) {
        print_usage(argv[0], opts_desc_generic);
        throw 1;
    }

    if (!var_map.count("input")) {
        cout << "No input file passed!\n" << endl;
        print_usage(argv[0], opts_desc_generic);
        throw 2;
    }

    vector<string> input_vector = var_map["input"].as<vector<string>>();
    if (input_vector.size() > 1) {
        cout << "Only one input file at a time is supported!\n" << endl;
        print_usage(argv[0], opts_desc_generic);
        throw 3;
    }

    namespace fs = filesystem;
    string file_path_string = input_vector[0];
    fs::path file_path(file_path_string);
    if (!fs::exists(file_path)) {
       cout << "The file " << file_path_string << " does not exist, or cannot be read!\n" << endl;
        print_usage(argv[0], opts_desc_generic);
        throw 4;
    }

    return var_map;
}

vector<work_unit> read_sudoku_file(const string& file) {
    vector<work_unit> units;
    ifstream input_file_stream(file);
    if (input_file_stream.is_open()) {
        int i = 0;  // index var for the work_unit
        string line;
        while (getline(input_file_stream, line)) {
            // filter empty lines and comments
            if (!(line.empty() || line.starts_with('#'))) {
                // create a work_unit (and increment the index)
                work_unit unit = work_unit(i++, line);
                // add it to the units vector
                units.push_back(unit);
            }
        }
        input_file_stream.close();
    }
    else cout << "Unable to open file " << file;

    return units;
}

// our mutex to control read (and pop) access to the work_unit vector
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

        // read unit string into sudoku and solve it
        Sudoku sudoku(unit.sudoku);
        sudoku.solve();

        // write the solution to the array of solved sudokus
        string solution = sudoku.to_string();
        solved[unit.index] = solution;

    } while (true);
}
vector<string> crunch(vector<work_unit>& units) {
    // because the threads pop from the vector, we need to remember the size at the start
    int unit_total = units.size();
    // get an array of strings, where the threads can write out the solutions
    auto* solved = new string[unit_total];

    // get the number of cores
    unsigned int cpus = thread::hardware_concurrency();
    if (cpus == 0) cpus++;  // seems like no concurrency supported, so only one thread
    const unsigned int thread_count = cpus;  // make it a const, probably unnecessary

    // our thread pool
    vector<thread> threads;
    for (int i = 0; i < thread_count; i++) {
        // instantiate our threads
        threads.emplace_back(do_work, ref(units), ref(solved));
    }

    // join the threads
    for (int i = 0; i < thread_count; i++) {
        if (threads[i].joinable()) {
            threads[i].join();
        }
    }

    // take the solved sudoku array, and turn it into a vector (fewer issues with pointer decay, and generally easier handling)
    vector<string> solved_vector = vector(solved, solved + unit_total);
    return solved_vector;
}
