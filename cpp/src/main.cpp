#include <iostream>
#include <string>
#include <vector>

#include "utils.hpp"

using namespace std;


int main() {

    int foo[3][3];

    for (int i = 0; i < 9; i++) {
        int x, y;
        div_t d = div(i, 3);

        x = d.rem;
        y = d.quot;

        foo[x][y] = x + y;
    }

    int x, y;
    x = y = 0;

    for (auto &row: foo) {
        for (auto &cell: row) {
            cout << "An Stelle (" << x << ", " << y << ") steht: " << cell << endl;
            y++;
        }
        y = 0;
        x++;
    }

    return 0;
}
