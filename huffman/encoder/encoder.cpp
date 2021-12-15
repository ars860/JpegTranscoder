#include "encoder.h"
#include "utils.h"
#include "entropy.h"

#include <iomanip>
#include <sstream>
#include <cassert>

// assuming data is padded correctly for ngram extraction
std::map<ngram, int> huffman::parse_dictionary(std::string &data) {
    assert (data.size() % NGRAM_SIZE == 0);

    std::map<ngram, int> ngram2cnt;
    for (int i = 0; i < data.size(); i += NGRAM_SIZE) {
        ngram2cnt[{{data.begin() + i, data.begin() + i + NGRAM_SIZE}}]++;
    }

    return ngram2cnt;
}

bit_vector huffman::encode_message(std::string &message, huffman_tree_node *tree, bool print_dictionary) {
    auto codes = tree->get_codes();

    if (print_dictionary) {
        std::cout << "Dictionary:\n";
        for (auto &[letter, code]: codes) {
            std::cout << letter.show() << " : " << code << '\n';
        }
    }

    bit_vector res;

    for (int i = 0; i < message.size(); i += NGRAM_SIZE) {
        ngram letter = {{message.begin() + i, message.begin() + i + NGRAM_SIZE}};
        std::string code = codes[letter];

        res.push_code(code);
    }

    return res;
}

std::pair<std::string, huffman::encode_report> huffman::encode(std::string &message, bool generate_report) {
    bit_vector res;

    bool has_additional_symbol = false;
    if (message.size() % NGRAM_SIZE != 0) {
        preprocess_message(message);
        has_additional_symbol = true;
    }

    res.push(has_additional_symbol);

    auto dict = parse_dictionary(message);
    auto tree = build_tree(dict);

    bit_vector tree_bit_vec = tree->encode_tree_with_letters();
    bit_vector message_bit_vec = encode_message(message, tree);

    uint8_t last_byte_free_bits_cnt = (8 - (4 + tree_bit_vec.size() + message_bit_vec.size()) % 8) % 8;

    res.push(last_byte_free_bits_cnt % 2);
    res.push((last_byte_free_bits_cnt >> 1) % 2);
    res.push((last_byte_free_bits_cnt >> 2) % 2);

    encode_report report{};
    report.message_size = message.size() * 8;
    report.total_bits_cnt = tree_bit_vec.size() + message_bit_vec.size() + res.size();
    report.message_bits_cnt = message_bit_vec.size();
    report.tree_bits_cnt = tree_bit_vec.size();
    report.helper_bits_cnt = res.size();
    report.code_speed_counting_helper_bits =
            (double) report.total_bits_cnt / (double) (message.size() - (has_additional_symbol ? 1 : 0));

    if (generate_report) {
        double speed = 0;
        size_t letters_cnt = message.size();
        for (auto &[letter, code]: tree->get_codes()) {
            speed += (double) code.size() * (double) dict[letter] / (double) letters_cnt;
        }

        report.code_speed_per_twogram = speed;
        report.code_speed_per_symbol = speed / 2;

        report.h_X = calc_entropy_hX(message);
        report.h_XIX = calc_entropy_hXIX(message);
        report.h_XX = calc_entropy_hXX(message);
        report.h_XIXX = calc_entropy_hXIXX(message);
    }

    res.push(tree_bit_vec);
    res.push(message_bit_vec);

    if (has_additional_symbol) {
        rollback_preprocess(message);
    }

    return {res.to_string(), report};
}

std::string huffman::encode_report::show() const {
    return "Message with length: " + std::to_string(message_size) + "bits, encoded with total: "
           + std::to_string(total_bits_cnt) + "bits:\n"
           + "Tree and letters: " + std::to_string(tree_bits_cnt) + " bits\n"
           + "Message: " + std::to_string(message_bits_cnt) + " bits\n"
           + "Helper bits: " + std::to_string(helper_bits_cnt) + " bits\n"
           + "Code speed per twogram: " + std::to_string(code_speed_per_twogram) + '\n'
           + "Code speed per symbol: " + std::to_string(code_speed_per_symbol) + '\n'
           + "Code speed per symbol counting tree and all: " + std::to_string(code_speed_counting_helper_bits) + '\n'
           + "H(X) = " + std::to_string(h_X) + '\n'
           + "H(X|X) = " + std::to_string(h_XIX) + '\n'
           + "H(XX) = " + std::to_string(h_XX) + '\n'
           + "H(X|XX) = " + std::to_string(h_XIXX) + '\n';
}

std::string huffman::encode_report::csv(const std::string &file_name) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(3)
           << file_name << ", " << h_X << ", " << h_XIX << ", " <<
           h_XX << ", " << h_XIXX << ", " << code_speed_per_twogram
           << ", " <<
           code_speed_per_symbol << ", " << code_speed_counting_helper_bits << ", " <<
           total_bits_cnt << ", " << total_bits_cnt / 8 + (total_bits_cnt % 8 > 0);
    return stream.str();
}
