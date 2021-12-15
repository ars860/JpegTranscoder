#include <iostream>
#include <cassert>
#include <filesystem>

#include "preprocess.h"
#include "encoder/encoder.h"
#include "utils.h"
#include "decoder/decoder.h"

using namespace std;

huffman::encode_report encode_decode_test(string &&test_message) {
    auto[encoded, report] = huffman::encode(test_message, true);
    string decoded = huffman::decode(encoded);

    assert(decoded == test_message);

    return report;
}

void encode_decode_simple_test() {
    string test_message = "IF_WE_CANNOT_DO_AS_WE_WOULD_WE_SHOULD_DO_AS_WE_CAN";
    cout << "Tesing on: " << test_message << '\n';
    encode_decode_test(std::move(test_message));
}

void write_read_file_test() {
    cout << "Testing read/write\n";

    string test_message = "IF_WE_CANNOT_DO_AS_WE_WOULD_WE_SHOULD_DO_AS_WE_CAN";
    auto[encoded, report] = huffman::encode(test_message, true);
    write_to_file(encoded, "test.txt");

    string from_file = read_file_to_string("test.txt");
    string decoded_from_file = huffman::decode(from_file);
    string decoded = huffman::decode(encoded);

    assert(test_message == decoded);
    assert(test_message == decoded_from_file);
}

int main() {
    write_read_file_test();
    encode_decode_simple_test();
    cout << '\n';


    string csv_report;
    bool csv = true;
    if (csv) {
        cout << huffman::encode_report::csv_header << '\n';
        csv_report += huffman::encode_report::csv_header;
        csv_report += '\n';
    }

    size_t total_bytes = 0;
    for (const auto &entry: filesystem::directory_iterator("test")) {
        if (!csv) {
            cout << "Testing file: " << entry.path().filename() << '\n';
        }
        string file_content = read_file_to_string(entry.path().string());
        auto report = encode_decode_test(std::move(file_content));

        total_bytes += report.total_bits_cnt / 8 + (report.total_bits_cnt % 8 != 0);

        cout << (csv ? report.csv(entry.path().filename().string()) : report.show()) << '\n';
        if (csv) {
            csv_report += report.csv(entry.path().filename().string());
            csv_report += '\n';
        }
    }
    cout << "Total bytes: " << total_bytes;

    if (csv) {
        write_to_file(csv_report, "../report.csv");
    }
}
