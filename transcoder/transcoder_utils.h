#ifndef TEORCOD_TRANSCODER_TRANSCODER_UTILS_H
#define TEORCOD_TRANSCODER_TRANSCODER_UTILS_H

#include <string>
#include <map>
#include <vector>

// encode integer in jpeg way
// returns code string -10 -> "0101"
std::string encode_value(int value);

// get cumulative frequencies from letter2frequency
std::map<uint64_t, uint64_t> cumulative_dict(const std::map<uint64_t, uint64_t> &letter2freq);

#endif //TEORCOD_TRANSCODER_TRANSCODER_UTILS_H