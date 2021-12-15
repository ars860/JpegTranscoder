#include <string>
#include <iostream>
#include "utils.h"
#include "trans_decoder.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./decoder <file_to_decode> <output_file>";
        return 0;
    }

    std::string input_file_name(argv[1]);
    std::string output_file_name(argv[2]);

    auto decoded = TransDecoder().file(input_file_name).decode();

    write_to_file(decoded, output_file_name);
}