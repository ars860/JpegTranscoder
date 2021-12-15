cmake_minimum_required(VERSION 3.16.3)
project(teorcod_transcoder)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -static)

include_directories("${PROJECT_SOURCE_DIR}/simple-jpeg-decoder/include/")
include_directories("${PROJECT_SOURCE_DIR}/huffman/")
include_directories("${PROJECT_SOURCE_DIR}/huffman/encoder")
include_directories("${PROJECT_SOURCE_DIR}/huffman/decoder")
include_directories("${PROJECT_SOURCE_DIR}/arithmetic_coding")
include_directories("${PROJECT_SOURCE_DIR}/transcoder")
include_directories("${PROJECT_SOURCE_DIR}/transcoder/encoder")
include_directories("${PROJECT_SOURCE_DIR}/transcoder/decoder")

set(HUFFMAN_SOURCES
        huffman/bit_vector.cpp huffman/bit_vector.h
        huffman/huffman_tree.cpp huffman/huffman_tree.h
        huffman/preprocess.cpp huffman/preprocess.h
        huffman/encoder/encoder.cpp huffman/encoder/encoder.h
        huffman/utils.cpp huffman/utils.h
        huffman/decoder/decoder.cpp huffman/decoder/decoder.h
        huffman/entropy.cpp huffman/entropy.h
        huffman/ngram.h huffman/ngram.cpp)

set(KPEG_SOURCES
        simple-jpeg-decoder/src/Decoder.cpp
        simple-jpeg-decoder/src/Image.cpp
        simple-jpeg-decoder/src/HuffmanTree.cpp
        simple-jpeg-decoder/src/MCU.cpp
        simple-jpeg-decoder/src/Transform.cpp
        simple-jpeg-decoder/src/Utility.cpp
        )

add_executable(kpeg simple-jpeg-decoder/main.cpp
        simple-jpeg-decoder/src/Decoder.cpp
        simple-jpeg-decoder/src/Image.cpp
        simple-jpeg-decoder/src/HuffmanTree.cpp
        simple-jpeg-decoder/src/MCU.cpp
        simple-jpeg-decoder/src/Transform.cpp
        simple-jpeg-decoder/src/Utility.cpp)
target_link_libraries(kpeg wsock32 ws2_32)

set(ARITHMETIC_SOURCES
        arithmetic_coding/arithmetic_encoder.h
        arithmetic_coding/arithmetic_encoder.cpp
        arithmetic_coding/arithmetic_decoder.h
        arithmetic_coding/arithmetic_decoder.cpp
        arithmetic_coding/consts.h)

add_executable(arithmetic_encoder
        huffman/bit_vector.cpp huffman/bit_vector.h
        ${ARITHMETIC_SOURCES}
        arithmetic_coding/main.cpp
        )
set_target_properties(arithmetic_encoder PROPERTIES LINKER_LANGUAGE CXX)

set(TRANSCODER_SOURCES
        transcoder/transcoder_utils.cpp transcoder/transcoder_utils.h
        )

add_executable(transcoder_test
        transcoder/test.cpp
        transcoder/encoder/trans_encoder.cpp transcoder/encoder/trans_encoder.h
        transcoder/decoder/trans_decoder.cpp transcoder/decoder/trans_decoder.h
        ${KPEG_SOURCES}
        ${ARITHMETIC_SOURCES}
        ${TRANSCODER_SOURCES}
        ${HUFFMAN_SOURCES}
        )
target_link_libraries(transcoder_test wsock32 ws2_32)

add_executable(encoder
        transcoder/encoder/main.cpp
        transcoder/encoder/trans_encoder.cpp transcoder/encoder/trans_encoder.h
        ${KPEG_SOURCES}
        ${ARITHMETIC_SOURCES}
        ${TRANSCODER_SOURCES}
        ${HUFFMAN_SOURCES}
        )
target_link_libraries(encoder wsock32 ws2_32)
target_link_options(encoder PRIVATE -static-libgcc -static-libstdc++)

add_executable(decoder
        transcoder/decoder/main.cpp
        transcoder/decoder/trans_decoder.cpp transcoder/decoder/trans_decoder.h
        ${KPEG_SOURCES}
        ${ARITHMETIC_SOURCES}
        ${TRANSCODER_SOURCES}
        ${HUFFMAN_SOURCES}
        )
target_link_libraries(decoder wsock32 ws2_32)
target_link_options(decoder PRIVATE -static-libgcc -static-libstdc++)

add_executable(huffman_test
        huffman/main.cpp
        ${HUFFMAN_SOURCES})