#ifndef TEORCOD_TRANSCODER_CONSTS_H
#define TEORCOD_TRANSCODER_CONSTS_H

#include <cstdint>

namespace arithmetic {
    const uint64_t k = 32; // 16
    const uint64_t R4 = 1ull << (k - 2);
    const uint64_t R2 = R4 * 2;
    const uint64_t R34 = R2 + R4;
    const uint64_t R = R2 * 2;
}

#endif //TEORCOD_TRANSCODER_CONSTS_H
