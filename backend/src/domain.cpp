#include "domain.h"

QString BusTypeToString(BusType bus_type) {
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