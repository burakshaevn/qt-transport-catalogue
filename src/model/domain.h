#pragma once

#ifndef DOMAIN_H
#define DOMAIN_H

#include <QString>
#include <vector>
#include <QDebug>
#include <QMetaEnum>

#include "geo.h"

template <typename EnumType>
EnumType StringToEnum(const QStringView str) {
    const QMetaEnum meta = QMetaEnum::fromType<EnumType>();
    bool ok;
    int value = meta.keyToValue(str.toString().toLatin1().constData(), &ok);
    if (!ok) {
        // Если не удалось преобразовать, возвращаем первое значение enum как значение по умолчанию
        value = meta.value(0);
    }
    return static_cast<EnumType>(value);
}

template <typename EnumType>
QString EnumToString(EnumType value) {
    const QMetaEnum meta = QMetaEnum::fromType<EnumType>();
    const char* key = meta.valueToKey(static_cast<int>(value));
    return key ? QString::fromLatin1(key) : QString();
}

std::vector<QString> SplitIntoWords(const QString& text);

#endif // DOMAIN_H
