#include <string>
#include <iostream>
#include "utils.h"
#include "trans_encoder.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./encoder <file_to_encode> <output_file>";
        return 0;
    }

    std::string input_file_name(argv[1]);
    std::string output_file_name(argv[2]);

    auto encoded = trans_encoder::encode(input_file_name);

    write_to_file(encoded, output_file_name);
}