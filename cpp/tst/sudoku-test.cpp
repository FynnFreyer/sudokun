#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <vector>
#include <string>

#include "utils.hpp"

using namespace std;

TEST_CASE("Computed sudokus are equal to dataset", "[full]") {
    vector<string> quizzes = read_sudoku_file("../../data/quizzes.sudoku");
    vector<string> solved = crunch(quizzes);

    vector<string> solutions = read_sudoku_file("../../data/solutions.sudoku");
    for (int i = 0; i < solutions.size(); i++) {
        REQUIRE(solved[i] == solutions[i]);
    }
}
