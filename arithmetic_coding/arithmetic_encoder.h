#ifndef TEORCOD_TRANSCODER_ARITHMETIC_ENCODER_H
#define TEORCOD_TRANSCODER_ARITHMETIC_ENCODER_H

#include <string>
#include <cmath>
#include <map>
#include <set>
#include <cassert>
#include "bit_vector.h"
#include "consts.h"

namespace arithmetic {

    void push_btf(bool bit, uint64_t &btf, bit_vector &vec);

    std::pair<std::vector<uint64_t>, std::vector<uint64_t>> get_sorted_freqnletters(const std::map<uint64_t,
            uint64_t> &letter2cnt);

    std::map<uint64_t, uint64_t> get_dict(const std::vector<uint64_t> &text);

    std::map<uint64_t, uint64_t> char_dict(size_t size = 256);

    bit_vector encode(const std::vector<uint64_t> &text, uint64_t init_m);

    bit_vector encode(const std::vector<uint64_t> &text, const std::map<uint64_t, uint64_t> &q, int64_t custom_m = -1);
}

#endif //TEORCOD_TRANSCODER_ARITHMETIC_ENCODER_H
