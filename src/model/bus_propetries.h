#pragma once

#ifndef BUS_PROPETRIES_H
#define BUS_PROPETRIES_H

#include <QColor>
#include <QObject> // Необходимо для Q_GADGET

#include "stop_properties.h"
#include "domain.h"

namespace BusProperties {

class BusEnums : public QObject {
    Q_GADGET
public:
    enum class Type {
        undefined,
        autobus,
        electrobus,
        trolleybus
    };
    Q_ENUM(Type)

    enum class OperatingTime {
        undefined,
        anytime,
        daytime,
        nighttime
    };
    Q_ENUM(OperatingTime)

    enum class Direction {
        undefined,
        circular,
        non_circular
    };
    Q_ENUM(Direction)
};

using Type = BusEnums::Type;
using OperatingTime = BusEnums::OperatingTime;
using Direction = BusEnums::Direction;

struct Bus {
    Bus() = default;
    Bus(const Bus&) = default;
    Bus& operator=(const Bus&) = default;
    Bus(Bus&&) = default;
    Bus& operator=(Bus&&) = default;

    QString name;
    std::vector<std::shared_ptr<const StopProperties::Stop>> stops;
    Direction direction;
    size_t color_index;
    QColor color;

    std::array<uint8_t, 3> rgb;
    Type type;
    int capacity;
    bool has_wifi;
    bool has_sockets;
    OperatingTime operating_time;

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
    Type type;
    int capacity;
    bool has_wifi;
    bool has_sockets;
    OperatingTime operating_time;

    bool is_available;
    int price;
};

} // namespace BusProperties

#endif // BUS_PROPETRIES_H
