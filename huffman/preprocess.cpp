#include "preprocess.h"
#include "ngram.h"

char ADDITIONAL_SYMBOL = '$';

std::string gen_appendix(size_t len) {
    std::string res = "";
    for (int i = 0; i < len; i++) {
        res += (char) (((unsigned char) ADDITIONAL_SYMBOL << i) + i * 13);
    }
    std::reverse(res.begin(), res.end());
    return res;
}

void preprocess_message(std::string &message) {
    message += gen_appendix((NGRAM_SIZE - (message.size() % NGRAM_SIZE)) % NGRAM_SIZE);
}

void rollback_preprocess(std::string &message) {
//    message.pop_back();
    int len = 1;
    std::string appendix = gen_appendix(len);
    while (message.compare(message.size() - len, len, appendix) == 0) {
        len++;
    }
    for (int i = 0; i < len - 1; i++) {
        message.pop_back();
    }
}
