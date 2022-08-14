#ifndef UTILS_HPP
#define UTILS_HPP

#include <utility>
#include <vector>
#include <string>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

po::variables_map parse_args(int argc, char** argv);

struct work_unit {
    work_unit(int index, std::string sudoku) : index(index), sudoku(std::move(sudoku)) {};

    int index;
    std::string sudoku;
};

std::vector<work_unit> read_sudoku_file(const std::string& file);
std::vector<std::string> crunch(std::vector<work_unit>& units);


#endif // UTILS_HPP
