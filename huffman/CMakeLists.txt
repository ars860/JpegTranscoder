cmake_minimum_required(VERSION 3.16.3)
project(teorcod)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -static)

add_executable(test
        main.cpp
        bit_vector.cpp bit_vector.h
        huffman_tree.cpp huffman_tree.h
        preprocess.cpp preprocess.h
        encoder/encoder.cpp encoder/encoder.h
        utils.cpp utils.h
        decoder/decoder.cpp decoder/decoder.h
        entropy.cpp entropy.h)

add_executable(encoder
        encoder/encoder.cpp encoder/encoder.h
        bit_vector.cpp bit_vector.h
        huffman_tree.cpp huffman_tree.h
        preprocess.cpp preprocess.h
        utils.cpp utils.h
        entropy.cpp entropy.h
        encoder/encoder_main.cpp)
target_link_options(encoder PRIVATE -static-libgcc -static-libstdc++)

add_executable(decoder
        decoder/decoder.cpp decoder/decoder.h
        bit_vector.cpp bit_vector.h
        huffman_tree.cpp huffman_tree.h
        preprocess.cpp preprocess.h
        utils.cpp utils.h
        decoder/decoder_main.cpp)
target_link_options(decoder PRIVATE -static-libgcc -static-libstdc++)