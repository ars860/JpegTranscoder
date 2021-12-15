#include <cassert>
#include "arithmetic_decoder.h"
#include "bit_vector.h"
#include "consts.h"

std::vector<uint64_t> arithmetic::decode(const bit_vector &bits, size_t sz, uint64_t init_m) {
//    std::map<uint64_t, char> restored_value2symbol;
//    for (auto [ch, cum]: q) {
//        restored_value2symbol[cum] = ch;
//    }

    std::vector<uint64_t> res;
    size_t bits_pos = 0;
    uint64_t m = init_m; // sz; // 0xFF + 1;

    uint64_t value = 0;
    for (size_t i = 0; i < k; ++i) {
        value = 2 * value + bits[bits_pos++];
    }

    uint64_t low = 0;
    uint64_t high = R - 1;

    for (int i = 0; i < sz; i++) {
        uint64_t range = high - low + 1;
        uint64_t restored_value = ((value - low + 1ull) * m - 1ull) / range;
        assert(restored_value <= m);

//        assert(restored_value <= 0xFF);
//        char real_symbol = restored_value2symbol.upper_bound((char)restored_value)->second;
//        auto prev = q.lower_bound(real_symbol);
//        --prev;
//        uint64_t start = prev == q.end() ? 0 : prev->second;

        high = low + ((range * (restored_value + 1)) / m) - 1;
        low = low + ((range * restored_value) / m);
        while (true) {
            if (high < R2) {
                // nothing
            } else if (low >= R2) {
                high -= R2;
                low -= R2;
                value -= R2;
            } else if (R4 <= low && high < R34) {
                high -= R4;
                low -= R4;
                value -= R4;
            } else {
                break;
            }

            high = (high << 1) | 1;
            low = low << 1;
            value = (value << 1) + bits[bits_pos++];
        }

        res.push_back(restored_value + 1);
        m = restored_value + 1;
    }

    return res;
}

std::vector<uint64_t> arithmetic::decode(bit_vector &bits,
                                         size_t sz,
                                         const std::map<uint64_t, uint64_t> &q,
                                         size_t custom_m) {
    std::map<uint64_t, uint64_t> restored_value2symbol;
    for (auto[ch, cum]: q) {
        restored_value2symbol[cum] = ch;
    }

//    bit_vector bits = bit_vector::from_string(text);
    std::vector<uint64_t> res;
    size_t bits_pos = 0;
    uint64_t m = sz; // 0xFF + 1;

    if (custom_m != -1) {
        m = custom_m;
    }

    uint64_t value = 0;
    for (size_t i = 0; i < k; ++i) {
        value = 2 * value + bits[bits_pos++];
    }

    uint64_t low = 0;
    uint64_t high = R - 1;

    for (int i = 0; i < sz; i++) {
        uint64_t range = high - low + 1;
        uint64_t restored_value = ((value - low + 1ull) * m - 1ull) / range;
        if (restored_value > m) {
            assert(restored_value <= m);
        }

        uint64_t real_symbol = restored_value2symbol.upper_bound(restored_value)->second;
        auto prev = q.lower_bound(real_symbol);
//        --prev;
        uint64_t start = prev == q.begin() ? 0 : (--prev)->second;

        high = low + ((range * q.at(real_symbol)) / m) - 1;
        low = low + ((range * start) / m);
        while (true) {
            if (high < R2) {
                // nothing
            } else if (low >= R2) {
                high -= R2;
                low -= R2;
                value -= R2;
            } else if (R4 <= low && high < R34) {
                high -= R4;
                low -= R4;
                value -= R4;
            } else {
                break;
            }

            high = (high << 1) | 1;
            low = low << 1;
            value = (value << 1) + bits[bits_pos++];
        }

        res.push_back(real_symbol);
    }

    return res;
}
