#include <string>
#include <iostream>
#include "utils.h"
#include "decoder.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./decoder <file_to_decode> <output_file>";
        return 0;
    }

    std::string input_file_name(argv[1]);
    std::string output_file_name(argv[2]);

    std::string file_content = read_file_to_string(input_file_name);

    std::string decoded = decode(file_content);

    write_to_file(decoded, output_file_name);
}