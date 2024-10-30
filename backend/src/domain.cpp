#include "domain.h"

std::string BusTypeToString(BusType bus_type) {
    switch (bus_type) {
    case BusType::autobus:
        return "autobus";
    case BusType::electrobus:
        return "electrobus";
    case BusType::trolleybus:
        return "trolleybus";
    default:
        return "undefined";
    }
}

std::vector<std::string> SplitIntoWords(const std::string& text) {
    std::vector<std::string> words;
    std::string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}