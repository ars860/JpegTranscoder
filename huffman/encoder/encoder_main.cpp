#include <string>
#include <iostream>
#include "encoder.h"
#include "utils.h"


int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./encoder <file_to_encode> <output_file>";
        return 0;
    }

    std::string input_file_name(argv[1]);
    std::string output_file_name(argv[2]);

    std::string file_content = read_file_to_string(input_file_name);

    auto [encoded, report] = encode(file_content);

    write_to_file(encoded, output_file_name);
}