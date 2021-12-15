#ifndef TEORCOD_BIT_VECTOR_H
#define TEORCOD_BIT_VECTOR_H

#include <vector>
#include <string>

class bit_vector {
public:
    bit_vector() = default;

    void push_code(const std::string &str);

    void pop();

    void push(bool bit);

    void push_byte(unsigned char byte);

    void push(const bit_vector &other);

    void push_back(bool bit);

    static bit_vector from_string(const std::string &s, size_t len = -1);

    static bit_vector from_substring(const std::string &s, size_t start = 0, size_t len = -1);

    bit_vector subbitvec(size_t start, size_t len) const;

    std::string to_string();

    std::string to_bit_string();

    bool operator[](size_t i) const;

    size_t size() const;

    // order of bits is inversed by default
    // should be called before writing to file
    // except cases, when further reading will
    // be done with bit_vector class
    void reverse_bytes();
private:
    bool is_bits_reversed = false;
    std::vector<unsigned char> content = {0};
    int pos_in_char = 0;
    int power_of_two = 1;
};

#endif //TEORCOD_BIT_VECTOR_H
