#ifndef TEORCOD_TRANSCODER_NGRAM_H
#define TEORCOD_TRANSCODER_NGRAM_H

#include <vector>
#include <string>

extern size_t NGRAM_SIZE;

struct ngram {
    std::string show() const;

    ngram() : data(NGRAM_SIZE) {}

    ngram(const std::vector<unsigned char> &data) : data(data) {}

    bool operator<(const ngram &rhs) const {
        return data < rhs.data;
    }

//    bool operator>(const ngram &rhs) const {
//        return rhs < *this;
//    }
//
//    bool operator<=(const ngram &rhs) const {
//        return !(rhs < *this);
//    }
//
//    bool operator>=(const ngram &rhs) const {
//        return !(*this < rhs);
//    }
    // operator< for maps
    // iterator?

    std::vector<unsigned char> data;
};

#endif //TEORCOD_TRANSCODER_NGRAM_H
