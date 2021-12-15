#include <iostream>
#include "arithmetic_encoder.h"
#include "arithmetic_decoder.h"

void vec_test_no_dict() {
    std::vector<uint64_t> test_vec = {500, 4, 3, 2, 1};
    bit_vector encoded_bits = arithmetic::encode(test_vec, 500);
    std::vector<uint64_t> decoded = arithmetic::decode(encoded_bits, 5, 500);
    std::string decoded_str(decoded.begin(), decoded.end());
//    std::cout << "Initial size: " << test_vec.size() * 8 << " Encoded size: " << encoded_bits.size() << '\n';
    assert(test_vec == decoded);
}

void string_test() {
    std::string test_str = "Test abacaba";
    std::vector<uint64_t> test_vec(test_str.begin(), test_str.end());
    auto dict = arithmetic::get_dict(test_vec);
    bit_vector encoded_bits = arithmetic::encode(test_vec, dict);
    std::vector<uint64_t> decoded = arithmetic::decode(encoded_bits, test_str.size(), dict);
    std::cout << "Initial size: " << test_vec.size() * 8 << " Encoded size: " << encoded_bits.size() << '\n';
    assert(decoded == test_vec);
}

void vec_test() {
    std::vector<uint64_t> test_vec = {12, 12, 3, 3, 4};
//    auto dict = get_dict(test_vec);
    std::map<uint64_t, uint64_t> dict = {{3, 500}, {4, 872}, {12, 1234}};
    bit_vector encoded_bits = arithmetic::encode(test_vec, dict, 1234);
//    encoded_bits.push_code("0101011");
    std::vector<uint64_t> decoded = arithmetic::decode(encoded_bits, test_vec.size(), dict, 1234);
//    std::cout << "Initial size: " << test_vec.size() * 8 << " Encoded size: " << encoded_bits.size() << '\n';
    assert(decoded == test_vec);
}

int main() {
//    std::string test = "ab";
//    bit_vector ab = bit_vector::from_string(test);
//    bit_vector a = bit_vector::from_substring(test, 2, 5);
//    assert(ab.to_bit_string().substr(2, 5) == a.to_bit_string());

    string_test();
    vec_test_no_dict();
    vec_test();
}