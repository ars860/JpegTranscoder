#include <cstddef>
#include <string>
#include "ngram.h"

// this is terrible
// should be template, but I am lazy
size_t NGRAM_SIZE = 2;

std::string ngram::show() const {
    std::string res = "";
    for (char ch: data) {
        res += ch;
    }
    return res;
}
