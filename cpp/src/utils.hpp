#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

po::variables_map parse_args(int argc, char** argv);
std::vector<std::string> split(std::string s, std::string sep, bool keep_sep = false);
std::vector<std::string> read_sudoku_file(const std::string& file);

#endif // UTILS_HPP
