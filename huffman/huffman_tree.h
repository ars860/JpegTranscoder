#ifndef TEORCOD_HUFFMAN_TREE_H
#define TEORCOD_HUFFMAN_TREE_H

#include <utility>
#include <memory>
#include <map>
#include <vector>
#include "bit_vector.h"
#include "ngram.h"

//typedef std::pair<char, char> twogram;

struct huffman_tree_node {
    std::unique_ptr<huffman_tree_node> children[2];

    huffman_tree_node() = default;

    std::string print() {
        return print_with_indent(0);
    }

    virtual ngram get_letter() {
        throw std::logic_error("not terminal node");
    }

    virtual std::map<ngram, std::string> get_codes();

    std::pair<bit_vector, std::vector<ngram>> encode_tree();

    bit_vector encode_tree_with_letters();

    static std::pair<huffman_tree_node *, size_t>
    decode_tree(std::string &encoded, int start_pos = 4, bool print_bits = false);

private:
    std::string print_with_indent(int indent);

    virtual std::string print_self() {
        return "node";
    }
};

struct huffman_tree_node_terminal : public huffman_tree_node {
    explicit huffman_tree_node_terminal() : huffman_tree_node() {}

    explicit huffman_tree_node_terminal(ngram letter) : huffman_tree_node(), letter(std::move(letter)) {}

    ngram get_letter() override {
        return letter;
    }

    std::string print_self() override {
        return "terminal[letter=" + letter.show() + "]";
    }

    std::map<ngram, std::string> get_codes() override {
        return {{letter, ""}};
    }

    ngram letter;
};

huffman_tree_node *build_tree(const std::map<ngram, int> &dict);

#endif //TEORCOD_HUFFMAN_TREE_H
