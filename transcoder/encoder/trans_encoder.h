#ifndef TEORCOD_TRANSCODER_TRANS_ENCODER_H
#define TEORCOD_TRANSCODER_TRANS_ENCODER_H

#include <string>
#include <vector>
#include <array>

namespace trans_encoder {
    std::string encode(const std::string& input_file);

    std::string encode(std::string &headers, std::vector <std::array<int, 64>> &zigzags, const std::string& initial_file);
};

#endif //TEORCOD_TRANSCODER_TRANS_ENCODER_H
