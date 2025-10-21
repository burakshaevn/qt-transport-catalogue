#include "domain.h"

std::vector<QString> SplitIntoWords(const QString& text) {
    std::vector<QString> words;
    QString word;
    for (const QChar c : text) {
        if (c == ' ') {
            if (!word.isEmpty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.isEmpty()) {
        words.push_back(word);
    }
    return words;
}
