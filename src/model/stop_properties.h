#pragma once

#ifndef STOP_PROPERTIES_H
#define STOP_PROPERTIES_H

#include "domain.h"

namespace StopProperties {

struct Stop {
    Stop() = default;
    template<typename T, typename U>
    Stop(const T&& name_, const U&& coords_)
        : name(std::forward<T>(name_))
        , coords(std::forward<U>(coords_))
    {}
    Stop(const QString& name_, const detail::Coordinates& coords_)
        : name(name_)
        , coords(coords_)
    {}
    QString name;
    detail::Coordinates coords;
};

}

#endif // STOP_PROPERTIES_H
