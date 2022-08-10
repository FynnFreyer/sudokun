#include <string>
#include <vector>

#include "utils.hpp"

using namespace std;


vector<string> split(string s, string sep, bool keep_sep) {
    // adapted from https://stackoverflow.com/a/14266139
    vector<string> split_list;

    size_t pos = 0;
    std::string token;
    while ((pos = s.find(sep)) != std::string::npos) {
        size_t eot = keep_sep ? pos + sep.length() : pos;  // end of token

        token = s.substr(0, eot);
        split_list.push_back(token);

        s.erase(0, pos + sep.length());
    }
    split_list.push_back(s);

    return split_list;
}
