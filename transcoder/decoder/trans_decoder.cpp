#include <cassert>
#include "trans_decoder.h"
#include "utils.h"
#include "arithmetic_decoder.h"
#include "arithmetic_encoder.h"
#include "transcoder_utils.h"
#include "decoder.h"

TransDecoder::TransDecoder() : jpeg_decoder(), content{} {
}

kpeg::Decoder::ResultCode TransDecoder::parse_headers() {
    jpeg_decoder.open(file_name);
    jpeg_decoder.start_from(headers_start / 8);
    jpeg_decoder.decode_only_header();

    auto result_code = jpeg_decoder.decodeImageFile();

    return result_code;
}

uint64_t TransDecoder::parse_x_bytes_value(size_t &pos, int x, const std::string &body) {
    uint64_t res = 0;
    std::string chars = bit_vector::from_substring(body, pos, x * 8).to_string();

    pos += x * 8;

    for (int i = 0; i < x; i++) {
        res += (((unsigned char) chars[i]) << (i * 8));
    }

    return res;
}

uint64_t TransDecoder::parse_two_bytes_value(size_t &pos, const std::string &body) {
    return parse_x_bytes_value(pos, 2, body);
}

uint64_t TransDecoder::parse_three_bytes_value(size_t &pos, const std::string &body) {
    return parse_x_bytes_value(pos, 3, body);
}

std::map<uint64_t, uint64_t> freqnletters2dict(const std::vector<uint64_t> &freq,
                                               const std::vector<uint64_t> &letters) {
    assert(freq.size() == letters.size());
    std::map<uint64_t, uint64_t> res;
    for (int i = 0; i < freq.size(); i++) {
        res[letters[i]] = freq[i];
    }

    return res;
}


std::vector<std::array<int, 64>> TransDecoder::decode_mcus_huffman_HXX(std::string &body) {
    size_t pos = 0;
    size_t huffman_encoded_size = parse_three_bytes_value(pos, body); // 19461

    std::string huffman_string = body.substr(pos / 8, huffman_encoded_size); // 3 64 32 68 -128
    pos += huffman_encoded_size * 8;

    std::string rle_decoded_str = huffman::decode(huffman_string);
    std::vector<unsigned char> rle_decoded_uchar = {rle_decoded_str.begin(), rle_decoded_str.end()};
    std::vector<uint64_t> rle_decoded = {rle_decoded_uchar.begin(), rle_decoded_uchar.end()};

    bit_vector zigzag_val = bit_vector::from_substring(body, pos, body.size() * 8 - pos);

    auto zigzags_decoded = decode_zigzags(rle_decoded, zigzag_val);

    return zigzags_decoded;
}

std::vector<std::array<int, 64>> TransDecoder::decode_mcus_numeric(std::string &body) {
    size_t pos = 0;
    uint64_t total_size = parse_three_bytes_value(pos, body);
    uint64_t freq_len = parse_two_bytes_value(pos, body);
    uint64_t freq_encoded_len = parse_two_bytes_value(pos, body);

    bit_vector freq_encoded = bit_vector::from_substring(body, pos, freq_encoded_len);
    pos += freq_encoded_len;

    std::vector<uint64_t> freq_decoded = arithmetic::decode(freq_encoded, freq_len, total_size);

    uint64_t letters_len = parse_two_bytes_value(pos, body);
    uint64_t letters_encoded_len = parse_two_bytes_value(pos, body);

    bit_vector letters_encoded = bit_vector::from_substring(body, pos, letters_encoded_len);
    pos += letters_encoded_len;

    std::vector<uint64_t> letters_decoded = arithmetic::decode(letters_encoded, letters_len, arithmetic::char_dict(),
                                                               256);

    auto letter2freq = freqnletters2dict(freq_decoded, letters_decoded);
    auto dict = cumulative_dict(letter2freq);

    uint64_t rle_encoded_len = parse_three_bytes_value(pos, body);

    bit_vector rle_encoded = bit_vector::from_substring(body, pos, rle_encoded_len);
    pos += rle_encoded_len;

    std::vector<uint64_t> rle_decoded = arithmetic::decode(rle_encoded, total_size, dict);

    bit_vector zigzag_val = bit_vector::from_substring(body, pos, body.size() * 8 - pos);

    auto zigzags_decoded = decode_zigzags(rle_decoded, zigzag_val);

    return zigzags_decoded;
}

std::vector<std::array<int, 64>>
TransDecoder::decode_zigzags(const std::vector<uint64_t> &rle_decoded, const bit_vector &zigzag_val) {
    for (uint64_t v: rle_decoded) {
        assert(v <= 0xff);
    }

    std::vector<std::array<int, 64>> res;
    res.emplace_back();

    size_t pos_in_vals = 0;
    size_t pos_in_cur_comp = 0;

    for (uint8_t cur: rle_decoded) {
        if (cur == 0) {
            if (pos_in_cur_comp == 0) {
                res[res.size() - 1][pos_in_cur_comp++] = 0;
            } else {
                for (; pos_in_cur_comp < 64; pos_in_cur_comp++) {
                    res[res.size() - 1][pos_in_cur_comp] = 0;
                }
            }
        } else {
            // first 4 bits is value len in bits
            int value_bits_cnt = cur & 0xf;
            // second 4 bits is zeros cnt
            int zeros_cnt = (cur >> 4) & 0xf;

            for (int i = 0; i < zeros_cnt; i++) {
                res[res.size() - 1][pos_in_cur_comp++] = 0;
            }

            // read value from bits
            int16_t value = kpeg::bitStringtoValue(zigzag_val.subbitvec(pos_in_vals, value_bits_cnt).to_bit_string());
            pos_in_vals += value_bits_cnt;
            res[res.size() - 1][pos_in_cur_comp++] = value;
        }

        if (pos_in_cur_comp == 64) {
            // go to next component
            res.emplace_back();
            pos_in_cur_comp = 0;
        }
    }

    res.pop_back();
    return res;
}

std::string TransDecoder::decode() {
    if (try_parse_raw() == kpeg::Decoder::ResultCode::DECODE_DONE) {
        return content;
    }

    size_t header_len = (uint8_t)content[0] + ((uint8_t)content[1] << 8);
    int pos = 16;

//    bit_vector bitvec = bit_vector::from_string(content);
//    unsigned char header_len_bits = content[0];
//    size_t header_len = 0;
//    int pos = 8;
//    for (int pov = 1; pos < 8 + header_len_bits; pos++, pov *= 2) {
//        if (bitvec[pos]) {
//            header_len += pov;
//        }
//    }
//
//    // align
//    while (pos % 8 != 0) {
//        pos++;
//    }

    headers_start = pos;
    // Parse headers
    auto result_code = parse_headers();
    assert(result_code == kpeg::Decoder::ResultCode::DECODE_DONE);
    pos += header_len * 8; // headers

    unsigned char type = content[pos / 8];
    pos += 8;

    std::string body = content.substr(pos / 8, content.size() - pos / 8);

    auto mcus_zigzags_decoded = type == 0 ? decode_mcus_huffman_HXX(body) : decode_mcus_numeric(body);

    bit_vector result;
    bit_vector encoded_mcus = huffman_encode_mcus(mcus_zigzags_decoded);
    bit_vector headers = bit_vector::from_string(jpeg_decoder.headers);

    // remove 2 bytes for some reason
    int test_pop = 8 * 2;
    for (int i = 0; i < test_pop; i++) {
        headers.pop();
    }

    result.push(headers);
    result.push(encoded_mcus);

    return result.to_string();
//    write_to_file(result.to_string(), output_name);
}

bit_vector TransDecoder::huffman_encode_mcus(std::vector<std::array<int, 64>> &mcus) {
    // assuming there are always only 4 tables
    std::map<char, std::string> type2tableId2codes[2][2] = {
            {{jpeg_decoder.m_huffmanTree[0][0].rle2code()}, {jpeg_decoder.m_huffmanTree[0][1].rle2code()}},
            {{jpeg_decoder.m_huffmanTree[1][0].rle2code()}, {jpeg_decoder.m_huffmanTree[1][1].rle2code()}}
    };

    bit_vector res;
    for (int mcu_index = 0; mcu_index < mcus.size(); ++mcu_index) {
        int compID = mcu_index % jpeg_decoder.mcu_comp_cnt;
        int HuffTableID = compID >= jpeg_decoder.mcu_comp_cnt - 2 ? 1 : 0;
        auto &mcu = mcus[mcu_index];

        int last_non_zero_pos = mcu.size() - 1;
        while (last_non_zero_pos > -1 && mcu[last_non_zero_pos] == 0) {
            last_non_zero_pos--;
        }

        bit_vector mcu_bits;
        int zeros_cnt = 0;
        for (int i = 0; i <= last_non_zero_pos; i++) {
            if (mcu[i] == 0 && zeros_cnt < 0xf) {
                if (i == 0) {
                    // EOB if DC is 0
                    assert(jpeg_decoder.m_huffmanTree[kpeg::HT_DC][HuffTableID].contains(
                            type2tableId2codes[kpeg::HT_DC][HuffTableID][0]) == "EOB");

                    std::string eob_code = type2tableId2codes[kpeg::HT_DC][HuffTableID][0];
                    mcu_bits.push_code(eob_code);
                } else {
                    zeros_cnt++;
                }
                continue;
            }
            auto &codes = type2tableId2codes[i == 0 ? kpeg::HT_DC : kpeg::HT_AC][HuffTableID];

            // parse value
            std::string value_bits = encode_value(mcu[i]);
            int value_bits_cnt = value_bits.size();

            assert(value_bits_cnt <= 0xf);
            assert(zeros_cnt <= 0xf);

            // first 4 bytes is value len in bits
            // second 4 bytes is zeros cnt
            unsigned char encoded = value_bits_cnt + (zeros_cnt << 4);
            // encode with huffman
            assert(codes.find((char) encoded) != codes.end());
            std::string encoded_bits = codes[(char) encoded];
            mcu_bits.push_code(encoded_bits);

            mcu_bits.push_code(value_bits);

            zeros_cnt = 0;
        }

        // insert EOB for DC if necessary
        if (last_non_zero_pos == -1) {
            assert(jpeg_decoder.m_huffmanTree[kpeg::HT_DC][HuffTableID].contains(
                    type2tableId2codes[kpeg::HT_DC][HuffTableID][0]) == "EOB");

            std::string eob_code = type2tableId2codes[kpeg::HT_DC][HuffTableID][0];
            mcu_bits.push_code(eob_code);
        }

        // insert EOB for AC if necessary
        if (last_non_zero_pos != 63) {
            assert(jpeg_decoder.m_huffmanTree[kpeg::HT_AC][HuffTableID].contains(
                    type2tableId2codes[kpeg::HT_AC][HuffTableID][0]) == "EOB");

            std::string eob_code = type2tableId2codes[kpeg::HT_AC][HuffTableID][0];
            mcu_bits.push_code(eob_code);
        }

        res.push(mcu_bits);
    }

    // fill rest with ones
    while (res.size() % 8 != 0) {
        res.push(1);
    }

    res.reverse_bytes();
    return byte_stuff(res);
}

bit_vector TransDecoder::byte_stuff(bit_vector &encoded) {
    std::string code = encoded.to_string();
    bit_vector code_test = bit_vector::from_string(code);
    std::string processed_code;
    for (int i = 0; i < code.size(); i++) {
        processed_code += code[i];

        if ((unsigned char) code[i] == 0xFF) {
            processed_code += (char) 0x00;
        }
    }

    // insert 0xFF 0xD9 after everything
    processed_code.push_back((char) 0xFF);
    processed_code.push_back((char) 0xD9);

    return bit_vector::from_string(processed_code);
}

TransDecoder &TransDecoder::file(const std::string &fileName) {
    this->file_name = fileName;
    content = read_file_to_string(fileName);
    return *this;
}

kpeg::Decoder::ResultCode TransDecoder::try_parse_raw() {
    kpeg::Decoder tmp_decoder;
    tmp_decoder.open(file_name);
//    tmp_decoder.start_from(headers_start / 8);
//    tmp_decoder.decode_only_header();

    auto result_code = tmp_decoder.decodeImageFile();

    return result_code;
}

