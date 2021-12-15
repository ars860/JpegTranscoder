#ifndef TEORCOD_TRANSCODER_TRANS_DECODER_H
#define TEORCOD_TRANSCODER_TRANS_DECODER_H

#include <string>
#include <vector>
#include <array>
#include "bit_vector.h"
#include "Decoder.hpp"

class TransDecoder {
public:
    TransDecoder();

    TransDecoder &file(const std::string &fileName);

    std::string decode();

private:
    kpeg::Decoder::ResultCode parse_headers();

    kpeg::Decoder::ResultCode try_parse_raw();

    static uint64_t parse_x_bytes_value(size_t &pos, int x, const std::string &body);

    static uint64_t parse_two_bytes_value(size_t &pos, const std::string &body);

    static uint64_t parse_three_bytes_value(size_t &pos, const std::string &body);

    std::vector<std::array<int, 64>> decode_mcus_huffman_HXX(std::string &body);

    std::vector<std::array<int, 64>> decode_mcus_numeric(std::string &body);

    static std::vector<std::array<int, 64>> decode_zigzags(const std::vector<uint64_t> &rle_decoded,
                                                           const bit_vector &zigzag_val);

    static std::vector<std::array<int, 64>> decode_zigzags(const std::vector<uint64_t> &rle_decoded_DC,
                                                           const std::vector<uint64_t> &rle_decoded_AC,
                                                           const bit_vector &zigzag_val);

    bit_vector huffman_encode_mcus(std::vector<std::array<int, 64>> &mcus);

    // replace every 0xFF with 0xFF 0x00
    // insert 0xFF 0xD9 after everything
    static bit_vector byte_stuff(bit_vector &encoded);

private:
    std::string file_name;
    std::string content;
    int headers_start;
    kpeg::Decoder jpeg_decoder;
};

#endif //TEORCOD_TRANSCODER_TRANS_DECODER_H
