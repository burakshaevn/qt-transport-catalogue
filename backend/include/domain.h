﻿#pragma once

#include <QString>
#include <vector> 
#include "geo.h"
#include <array>

struct Stop {
    Stop() = default;
    QString name;
    detail::Coordinates coords; 

    bool operator<(const Stop& other) const {
        return name < other.name;
    }
};

enum class BusType {
    undefined,
    autobus,
    electrobus,
    trolleybus 
};

QString BusTypeToString(BusType bus_type);

template <typename T>
BusType StringToBusType(const T bus_type) {
    if (bus_type == "autobus") {
        return BusType::autobus;
    }
    else if (bus_type == "electrobus") {
        return BusType::electrobus;
    }
    else if (bus_type == "trolleybus") {
        return BusType::trolleybus;
    }
    else {
        return BusType::undefined;
    }
}

std::vector<QString> SplitIntoWords(const QString& text);

struct Bus {
    QString name;
    std::vector<const Stop*> stops;
    bool is_roundtrip;
    size_t color_index;

    std::array<uint8_t, 3> rgb;
    BusType bus_type;
    uint8_t capacity;
    bool has_wifi;
    bool has_sockets;
    bool is_night;

    bool is_available;
    uint8_t price;
};

struct BusInfo {
    QString name;
    size_t count_stops;
    size_t unique_count_stops;
    double len;
    double curvature;

    std::array<uint8_t, 3> rgb;
    BusType bus_type;
    uint8_t capacity;
    bool has_wifi;
    bool has_sockets;
    bool is_night;

    bool is_available;
    uint8_t price;
};
