#include <string>
#include <filesystem>
#include <cassert>
#include "trans_encoder.h"
#include "trans_decoder.h"
#include "utils.h"

int main() {
    for (const std::string& dir_name: {"jpeg30", "jpeg80"}) {
        std::cout << "Looking at: " << dir_name << '\n';

        double mean_win = 0;
        double files_cnt = 0;

        for (const auto &entry: std::filesystem::directory_iterator(dir_name)) {
            std::string file_content = read_file_to_string(entry.path().string());
            auto encoded = trans_encoder::encode(entry.path().string());
            write_to_file(encoded, "encoded");
            auto decoded = TransDecoder().file("encoded").decode();
            write_to_file(decoded, "decoded.jpeg");

            assert(decoded == file_content);

            double win = (double) ((int)decoded.size() - (int)encoded.size()) / (double) decoded.size();

            std::cout << "File: " << entry.path().filename() << "encoded size: " << encoded.size() << " decoded size: "
                      << decoded.size()
                      << " win: " << win << '\n';
            mean_win += win;
            files_cnt++;
        }

        mean_win /= files_cnt;

        std::cout << "Mean win: " << mean_win << '\n';
    }
}