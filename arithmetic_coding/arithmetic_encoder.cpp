#include "arithmetic_encoder.h"

void arithmetic::push_btf(bool bit, uint64_t &btf, bit_vector &vec) {
    vec.push(bit);
    for (size_t i = 0; i < btf; ++i) {
        vec.push(!bit);
    }
    btf = 0;
}

std::pair<std::vector<uint64_t>, std::vector<uint64_t>>
arithmetic::get_sorted_freqnletters(const std::map<uint64_t, uint64_t> &letter2cnt) {
    std::vector<std::pair<uint64_t, uint64_t>> freq2letter;
    freq2letter.reserve(letter2cnt.size());
    for (auto[letter, freq]: letter2cnt) {
//        assert(letter <= 0xFF);
        freq2letter.emplace_back(freq, letter);
    }
    std::sort(freq2letter.begin(), freq2letter.end(), std::greater());


    std::vector<uint64_t> freqs;
    std::vector<uint64_t> letters;
    for (auto[freq, letter]: freq2letter) {
        freqs.push_back(freq);
        letters.push_back(letter);
    }

    return {freqs, letters};
}

std::map<uint64_t, uint64_t> arithmetic::get_dict(const std::vector<uint64_t> &text) {
    std::map<uint64_t, uint64_t> letter2cnt;

    for (uint64_t ch: text) {
        letter2cnt[ch]++;
    }

    std::map<uint64_t, uint64_t> res;

    uint64_t add = 0;
    for (auto[letter, cnt]: letter2cnt) {
        add += cnt;
        res[letter] = add;
    }

    return res;
}

std::map<uint64_t, uint64_t> arithmetic::char_dict(size_t size) {
    std::map<uint64_t, uint64_t> res;
    for (int ch = 0; ch < size; ch++) {
        res[ch] = ch + 1;
    }
    return res;
}

bit_vector arithmetic::encode(const std::vector<uint64_t> &text, uint64_t init_m) {
    bit_vector res;
    uint64_t m = init_m; //0xff + 1;

    uint64_t low = 0;
    uint64_t high = R - 1;
    size_t btf = 0;

    for (uint64_t ch: text) {
        if (ch > m) {
            assert(ch <= m);
        }
        ch--;

        uint64_t range = high - low + 1;
        high = low + ((range * (ch + 1)) / m) - 1;
        low = low + ((range * ch) / m);

        while (true) {
            if (high < R2) {
                push_btf(0, btf, res);
            } else if (low >= R2) {
                push_btf(1, btf, res);

                high -= R2;
                low -= R2;
            } else if (R4 <= low && high < R34) {
                btf++;
                high -= R4;
                low -= R4;
            } else {
                break;
            }

            high = (high << 1) | 1ull;
            low = low << 1;
        }

        m = ch + 1;
    }

    btf++;
    push_btf(low >= R4, btf, res);

    return res;
}

bit_vector arithmetic::encode(const std::vector<uint64_t> &text,
                              const std::map<uint64_t, uint64_t> &q,
                              int64_t custom_m) {
    bit_vector res;
    uint64_t m = text.size(); // 0xff + 1;

    // kostyl
    if (custom_m != -1) {
        m = custom_m;
    }

    uint64_t low = 0;
    uint64_t high = R - 1;
    size_t btf = 0;

    for (uint64_t ch: text) {
        auto prev = q.lower_bound(ch);
//        --prev;
        uint64_t start = prev == q.begin() ? 0 : (--prev)->second;

        if (q.find(ch) == q.end()) {
            assert(q.find(ch) != q.end());
        }

        uint64_t range = high - low + 1;
        high = low + ((range * q.at(ch)/*(ch + 1)*/) / m) - 1;
        low = low + ((range * start/*ch*/) / m);

        while (true) {
            if (high < R2) {
                push_btf(0, btf, res);
            } else if (low >= R2) {
                push_btf(1, btf, res);

                high -= R2;
                low -= R2;
            } else if (R4 <= low && high < R34) {
                btf++;
                high -= R4;
                low -= R4;
            } else {
                break;
            }

            high = (high << 1) | 1ull;
            low = low << 1;
        }
    }

    btf++;
    push_btf(low >= R4, btf, res);

    return res;
}
