#pragma once

#include <QString>
#include <vector> 
#include "geo.h"
#include <array>

struct Stop { 
    QString name;
    detail::Coordinates coords; 
     
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
    Bus() = default;  
    Bus(const Bus&) = default;
    Bus& operator=(const Bus&) = default;
    Bus(Bus&&) = default;
    Bus& operator=(Bus&&) = default;

    QString name;
    std::vector<const Stop*> stops;
    bool is_roundtrip;
    size_t color_index;

    std::array<uint8_t, 3> rgb;
    BusType bus_type;
    int capacity;
    bool has_wifi;
    bool has_sockets;
    bool is_night;
    bool is_day;

    bool is_available;
    int price;
};

struct BusInfo {
    QString name;
    size_t count_stops;
    size_t unique_count_stops;
    double len;
    double curvature;

    std::array<uint8_t, 3> rgb;
    BusType bus_type;
    int capacity;
    bool has_wifi;
    bool has_sockets;
    bool is_night;
    bool is_day;

    bool is_available;
    int price;
};
