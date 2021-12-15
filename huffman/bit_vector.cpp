#include <bitset>
#include <cassert>
#include "bit_vector.h"

void bit_vector::push(bool bit) {
    assert(!is_bits_reversed);

    if (pos_in_char > 7) {
        content.push_back(0);
        pos_in_char = 0;
        power_of_two = 1;
    }

    content[content.size() - 1] = (char) ((unsigned char) content[content.size() - 1] + power_of_two * (bit ? 1 : 0));
    power_of_two <<= 1;
    pos_in_char++;
}

void bit_vector::push_code(const std::string &str) {
    for (char ch: str) {
        push(ch == '1');
    }
}

bit_vector bit_vector::from_string(const std::string &s, size_t len) {
    if (len == -1) {
        len = 8 * s.size();
    }

    bit_vector res;
    for (char ch: s) {
        for (int i = 0; i < std::min((size_t) 8, len); i++) {
            res.push(((unsigned char) ch >> i) % 2 == 1);
        }
        len -= 8;
    }

    return res;
}

bit_vector bit_vector::from_substring(const std::string &s, size_t start, size_t len) {
    if (len == -1) {
        len = 8 * s.size() - start;
    }

    bit_vector res;
    for (size_t pos = start; pos < start + len; pos++) {
        res.push(((unsigned char) s[pos / 8] >> (pos % 8)) % 2 == 1);
    }

    return res;
}

std::string bit_vector::to_string() {
    return {content.begin(), content.end()};
}

bool bit_vector::operator[](size_t i) const {
    // for numeric coding
    if (i >= size()) {
        return 0;
    }

    return ((unsigned char) content[i / 8] >> (i % 8)) % 2;
}

size_t bit_vector::size() const {
    return (content.size() - 1) * 8 + pos_in_char;
}

void bit_vector::push_back(bool bit) {
    push(bit);
}

std::string bit_vector::to_bit_string() {
    std::vector<char> res(size());
    for (int i = 0; i < size(); i++) {
        res[i] = (*this)[i] ? '1' : '0';
    }

    return {res.begin(), res.end()};
}

void bit_vector::push(const bit_vector &other) {
    for (int i = 0; i < other.size(); i++) {
        push(other[i]);
    }
}

void bit_vector::push_byte(unsigned char byte) {
    for (int i = 0; i < 8; i++) {
        push(((unsigned char) byte >> i) % 2 == 1);
    }
}

void bit_vector::pop() {
    assert(!is_bits_reversed);

    pos_in_char--;
    if (pos_in_char < 0) {
        pos_in_char = 7;
        content.pop_back();
    }
}

void bit_vector::reverse_bytes() {
    is_bits_reversed = !is_bits_reversed;
    for (int i = 0; i < content.size(); i++) {
        unsigned char old = content[i];
        content[i] = (char) (128 * (old % 2) + 64 * ((old & 0x02) != 0) + 32 * ((old & 0x04) != 0) +
                             16 * ((old & 0x08) != 0) +
                             8 * ((old & 0x10) != 0) + 4 * ((old & 0x20) != 0) +
                             2 * ((old & 0x40) != 0) + ((old & 0x80) != 0));
    }
}

bit_vector bit_vector::subbitvec(size_t start, size_t len) const {
    bit_vector res;

    for (size_t i = start; i < start + len; i++) {
        res.push((*this)[i]);
    }

    return res;
}

