#ifndef UTILS_HPP
#define UTILS_HPP

#include <utility>
#include <vector>
#include <string>

std::vector<std::string> read_sudoku_file(const std::string& file);
std::vector<std::string> crunch(std::vector<std::string>& sudokus);

#endif // UTILS_HPP
