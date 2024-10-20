// ��� ����������� �������� � ����, ����������� ������ 

#include "request_handler.h"
#include <algorithm> 

std::optional<BusInfo> RequestHandler::GetBusStat(const std::string_view bus_number) const {
    BusInfo bus_stat{};
    //const Bus* bus = catalogue_.FindBus(bus_number);
    std::optional<Bus> bus_opt = catalogue_.FindBus(bus_number);
    if (!bus_opt.has_value()) {
        throw std::invalid_argument("Bus not found");
    }

    const Bus* bus = &(*bus_opt);  // Получаем указатель на значение внутри std::optional

    if (!bus) throw std::invalid_argument("bus not found");

    bus_stat.count_stops = bus->is_roundtrip ? bus->stops.size() : bus->stops.size() * 2 - 1;

    int route_length = 0;
    double geographic_length = 0.0;

    for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
        auto from = bus->stops[i];
        auto to = bus->stops[i + 1];

        route_length += catalogue_.GetDistance(from, to);
        geographic_length += detail::ComputeDistance(from->coords, to->coords);

        if (!bus->is_roundtrip) {
            route_length += catalogue_.GetDistance(to, from);
            geographic_length += detail::ComputeDistance(to->coords, from->coords);
        }
    }

    bus_stat.unique_count_stops = catalogue_.ComputeUniqueStopsCount(bus_number);
    bus_stat.len = route_length;
    bus_stat.curvature = route_length / geographic_length;

    return bus_stat;
}

//svg::Document RequestHandler::RenderMap([[maybe_unused]] const std::string& bus_name) const {
//    if (bus_name.empty()) {
//        return renderer_.GetSVG(catalogue_.GetSortedBuses());
//    }
//    else {
//        return renderer_.GetSVG(catalogue_.GetSortedBuses(bus_name));
//    }
//}

svg::Document RequestHandler::RenderMap([[maybe_unused]] const std::string_view bus_name) const {
    // Преобразование карты автобусов в карту указателей на автобусы
    auto convertToPointerMap = [](const std::map<std::string_view, Bus>& buses) {
        std::map<std::string_view, const Bus*> pointer_map;
        for (const auto& [bus_name, bus] : buses) {
            pointer_map[bus_name] = &bus;  // Преобразование в указатель
        }
        return pointer_map;
        };

    if (bus_name.empty()) {
        // Получаем отсортированные автобусы и преобразуем их в карту указателей
        const auto buses = catalogue_.GetSortedBuses();
        return renderer_.GetSVG(convertToPointerMap(buses));
    }
    else {
        // Получаем отсортированный автобус по имени и преобразуем в карту указателей
        const auto buses = catalogue_.GetSortedBuses(bus_name);
        return renderer_.GetSVG(convertToPointerMap(buses));
    }
}


const TransportCatalogue& RequestHandler::GetCatalogue() const {
    return catalogue_;
}

std::pair<std::optional<graph::Router<double>::RouteInfo>, const TransportRouter::Graph&>
RequestHandler::GetOptimalRoute(const std::string_view from, const std::string_view to) const {
    auto route_info_opt = transport_router_.FindRoute(from, to);
    if (!route_info_opt) {
        return { std::nullopt, transport_router_.graph_ };
    }
    return { *route_info_opt, transport_router_.graph_ };
}