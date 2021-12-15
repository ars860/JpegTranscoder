#include <map>
#include <cassert>
#include "trans_encoder.h"
#include "utils.h"
#include "bit_vector.h"
#include "transcoder_utils.h"
#include "arithmetic_encoder.h"
#include "Decoder.hpp"
#include "encoder.h"
#include "decoder.h"


std::string trans_encoder::encode(std::string &headers, std::vector<std::array<int, 64>> &zigzags,
                                  const std::string &initial_file) {
    bit_vector result;

    result.push_byte(headers.size() & 0xff);
    result.push_byte((headers.size() >> 8) & 0xff);

    result.push(bit_vector::from_string(headers, headers.size() * 8));

    // encode with numeric coding
    std::map<uint64_t, uint64_t> all_freq;

    std::vector<uint64_t> zigzag_rle;
    bit_vector zigzag_val;

    std::vector<char> huffman_HXX_coef;
    std::vector<char> huffman_HXX_DC;
    std::vector<char> huffman_HXX_AC;

    for (int mcu_index = 0; mcu_index < zigzags.size(); ++mcu_index) {
        auto &zigzag = zigzags[mcu_index];

        int last_non_zero_pos = zigzag.size() - 1;
        while (last_non_zero_pos > -1 && zigzag[last_non_zero_pos] == 0) {
            last_non_zero_pos--;
        }

        int zeros_cnt = 0;
        for (int i = 0; i <= last_non_zero_pos; i++) {
            if (zigzag[i] == 0 && zeros_cnt < 0xf) {
                if (i == 0) {
                    zigzag_rle.push_back(0);
                    huffman_HXX_coef.push_back(0);
                    huffman_HXX_DC.push_back(0);
                } else {
                    zeros_cnt++;
                }
                continue;
            }

            // parse value
            std::string value_bits = encode_value(zigzag[i]);
            size_t value_bits_cnt = value_bits.size();

            assert(value_bits_cnt <= 0xf);
            assert(zeros_cnt <= 0xf);

            // first 4 bytes is value len in bits
            // second 4 bytes is zeros cnt
            unsigned char encoded = value_bits_cnt + (zeros_cnt << 4);

            zigzag_rle.push_back(encoded);
            zigzag_val.push_code(value_bits);

            huffman_HXX_coef.push_back(encoded);
            if (i == 0) {
                huffman_HXX_DC.push_back(encoded);
            } else {
                huffman_HXX_AC.push_back(encoded);
            }

            zeros_cnt = 0;
        }

        // insert EOB for DC if necessary
        if (last_non_zero_pos == -1) {
            zigzag_rle.push_back(0);
            huffman_HXX_coef.push_back(0);
            huffman_HXX_DC.push_back(0);
        }

        // insert EOB for AC if necessary
        if (last_non_zero_pos != 63) {
            zigzag_rle.push_back(0);
            huffman_HXX_coef.push_back(0);
            huffman_HXX_AC.push_back(0);
        }
    }

    for (uint64_t ch: zigzag_rle) {
        all_freq[ch]++;
    }

    std::map<uint64_t, uint64_t> dict = cumulative_dict(all_freq);
    uint64_t total_size = zigzag_rle.size();

    bit_vector numeric_result;

    // encode total_size
    assert(total_size <= 0xffffff);
    numeric_result.push_byte(total_size & 0x0000ff);
    numeric_result.push_byte((total_size >> 8) & 0x0000ff);
    numeric_result.push_byte((total_size >> 16) & 0x0000ff);

    auto[freq, letters] = arithmetic::get_sorted_freqnletters(all_freq);

    bit_vector freq_encoded = arithmetic::encode(freq, total_size);

    // encode freq len in two bytes
    uint64_t freq_len = freq.size();
    assert(freq_len <= 0xffff);
    numeric_result.push_byte(freq_len & 0x00ff);
    numeric_result.push_byte((freq_len >> 8) & 0x00ff);

    // encode freq_encoded.size()
    assert(freq_encoded.size() <= 0xffff);
    numeric_result.push_byte(freq_encoded.size() & 0x00ff);
    numeric_result.push_byte((freq_encoded.size() >> 8) & 0x00ff);

    // encode freq
    numeric_result.push(freq_encoded);

    bit_vector letters_encoded = arithmetic::encode(letters, arithmetic::char_dict(), 256);

    // encode letters len in two bytes
    uint64_t letters_len = freq.size();
    assert(letters_len <= 0xffff);
    numeric_result.push_byte(letters_len & 0x00ff);
    numeric_result.push_byte((letters_len >> 8) & 0x00ff);

    // encode letters_encoded.size()
    assert(letters_encoded.size() <= 0xffff);
    numeric_result.push_byte(letters_encoded.size() & 0x00ff);
    numeric_result.push_byte((letters_encoded.size() >> 8) & 0x00ff);

    // encode letters
    numeric_result.push(letters_encoded);

    // encode body
    bit_vector rle_encoded = arithmetic::encode(zigzag_rle, dict);

    bit_vector zigzag_encoded;
    assert(rle_encoded.size() < 0xffffff);
    zigzag_encoded.push_byte(rle_encoded.size() & 0xff);
    zigzag_encoded.push_byte((rle_encoded.size() >> 8) & 0xff);
    zigzag_encoded.push_byte((rle_encoded.size() >> 16) & 0xff);
    zigzag_encoded.push(rle_encoded);
    zigzag_encoded.push(zigzag_val);

    numeric_result.push(zigzag_encoded);

//    std::string huffman_message = {huffman_HXX_coef.begin(), huffman_HXX_coef.end()};
//    std::string huffman_encoded = huffman::encode(huffman_message).first;

    std::string huffman_message_DC = {huffman_HXX_DC.begin(), huffman_HXX_DC.end()};
    std::string huffman_encoded_DC = huffman::encode(huffman_message_DC).first;

    std::string huffman_message_AC = {huffman_HXX_AC.begin(), huffman_HXX_AC.end()};
    std::string huffman_encoded_AC = huffman::encode(huffman_message_AC).first;

    bit_vector huffman_result;
//    assert(huffman_encoded.size() < 0xffffff);
//    huffman_result.push_byte(huffman_encoded.size() & 0xff);
//    huffman_result.push_byte((huffman_encoded.size() >> 8) & 0xff);
//    huffman_result.push_byte((huffman_encoded.size() >> 16) & 0xff);
//    bit_vector huffman_bits = bit_vector::from_string(huffman_encoded);
//    huffman_result.push(huffman_bits);

    assert(huffman_encoded_DC.size() < 0xffffff);
    huffman_result.push_byte(huffman_encoded_DC.size() & 0xff);
    huffman_result.push_byte((huffman_encoded_DC.size() >> 8) & 0xff);
    huffman_result.push_byte((huffman_encoded_DC.size() >> 16) & 0xff);
    bit_vector huffman_bits_DC = bit_vector::from_string(huffman_encoded_DC);
    huffman_result.push(huffman_bits_DC);

    assert(huffman_encoded_AC.size() < 0xffffff);
    huffman_result.push_byte(huffman_encoded_AC.size() & 0xff);
    huffman_result.push_byte((huffman_encoded_AC.size() >> 8) & 0xff);
    huffman_result.push_byte((huffman_encoded_AC.size() >> 16) & 0xff);
    bit_vector huffman_bits_AC = bit_vector::from_string(huffman_encoded_AC);
    huffman_result.push(huffman_bits_AC);

    huffman_result.push(zigzag_val);

    std::string initial_file_content = read_file_to_string(initial_file);
    if (initial_file_content.size() < (result.size() + huffman_result.size()) / 8 &&
        initial_file_content.size() < (result.size() + numeric_result.size()) / 8) {
//        result.push_byte(0xfa);
        std::cout << "Raw encoded\n";
        return initial_file_content;
    }

    if (huffman_result.size() < numeric_result.size()) {
        std::cout << "Huffman encoded\n";
        result.push_byte(0x00);
        result.push(huffman_result);
    } else {
        std::cout << "Numeric encoded\n";
        result.push_byte(0xff);
        result.push(numeric_result);
    }

    return result.to_string();
}

std::string trans_encoder::encode(const std::string &input_file) {
    kpeg::Decoder decoder;
    decoder.open(input_file);

    if (decoder.decodeImageFile() != kpeg::Decoder::ResultCode::DECODE_DONE) {
        throw std::runtime_error("Broken jpeg");
    }

    auto zigzags = decoder.get_raw_zigzags();
    auto headers = decoder.headers;

    return encode(headers, zigzags, input_file);
}
