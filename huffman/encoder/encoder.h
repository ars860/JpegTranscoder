#ifndef TEORCOD_ENCODER_H
#define TEORCOD_ENCODER_H

#include <iostream>
#include "bit_vector.h"
#include "huffman_tree.h"
#include "preprocess.h"

namespace huffman {

    std::map<ngram, int> parse_dictionary(std::string &data);

    bit_vector encode_message(std::string &message, huffman_tree_node *tree, bool print_dictionary = false);

    struct encode_report {
        size_t message_size;

        size_t total_bits_cnt;
        size_t helper_bits_cnt;
        size_t tree_bits_cnt;
        size_t message_bits_cnt;

        double code_speed_per_twogram;
        double code_speed_per_symbol;
        double code_speed_counting_helper_bits;
        double h_X;
        double h_XIX;
        double h_XX;
        double h_XIXX;

        std::string show() const;

        std::string csv(const std::string &file_name);

        static inline const std::string csv_header = "file_name, H(X), H(X|X), H(XX), H(X|XX), speed_per_twogram, speed_per_symbol, speed_per_symbol_counting_tree, file_size_bits, file_size_bytes";

        explicit operator std::string() const {
            return show();
        };
    };

    std::pair<std::string, encode_report> encode(std::string &message, bool generate_report = false);

}

#endif //TEORCOD_ENCODER_H
