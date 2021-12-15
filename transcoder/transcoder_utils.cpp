#include <cassert>
#include "transcoder_utils.h"
#include "Transform.hpp"

std::string encode_value(int value) {
    int tmp_value = value;
    std::string value_bits;
    bool sign = tmp_value > 0;
    tmp_value = sign ? tmp_value : -tmp_value;
    while (tmp_value > 0) {
        value_bits += '0' + (tmp_value % 2 != 0 == sign);
        tmp_value /= 2;
    }
    std::reverse(value_bits.begin(), value_bits.end());

    assert(kpeg::bitStringtoValue(value_bits) == value);

    return value_bits;
}

std::map<uint64_t, uint64_t> cumulative_dict(const std::map<uint64_t, uint64_t> &letter2freq) {
    std::map<uint64_t, uint64_t> dict;
    uint64_t total_size = 0;
    for (auto[letter, cnt]: letter2freq) {
        total_size += cnt;
        dict[letter] = total_size;
    }

    return dict;
}