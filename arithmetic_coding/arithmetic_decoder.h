#ifndef TEORCOD_TRANSCODER_ARITHMETIC_DECODER_H
#define TEORCOD_TRANSCODER_ARITHMETIC_DECODER_H

#include <map>
#include "bit_vector.h"

namespace arithmetic {

    std::vector<uint64_t> decode(const bit_vector &bits, size_t sz, uint64_t init_m);

    std::vector<uint64_t> decode(bit_vector &bits,
                                 size_t sz,
                                 const std::map<uint64_t, uint64_t> &q,
                                 size_t custom_m = -1);
}

#endif //TEORCOD_TRANSCODER_ARITHMETIC_DECODER_H
