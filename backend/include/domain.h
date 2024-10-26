#pragma once

#include <string>
#include <vector> 
#include "geo.h"
#include <array>

struct Stop {
    Stop() = default;
    std::string name;
    detail::Coordinates coords; 

    bool operator<(const Stop& other) const {
        return name < other.name;
    }
};

enum class BusType {
    Undefined,
    Autobus,
    Electrobus,
    Trolleybus
};

struct Bus {
    std::string name;
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
    std::string name;
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
