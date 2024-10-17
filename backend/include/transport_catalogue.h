#pragma once

#include "domain.h"
#include <deque>
#include <iostream>
#include <set> 
#include <map> 
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "domain.h"
#include "geo.h"
#include <algorithm>

using namespace std::string_literals;
using namespace std::string_view_literals;

// класс транспортного справочника 

struct StopHasher {
	size_t operator()(const std::pair<const Stop*, const Stop*>& points) const {
		size_t hash_first = std::hash<const void*>{}(points.first);
		size_t hash_second = std::hash<const void*>{}(points.second);
		return hash_first + hash_second * 37;
	}
};

class TransportCatalogue {
public:
    void AddBus(const std::string_view name,
                std::vector<const Stop*>& stops,
                const bool is_roundtrip,
                const size_t color_index,
                const std::array<uint8_t, 3> rgb,
                const BusType bus_type,
                const uint8_t capacity,
                const bool has_wifi,
                const bool has_sockets,
                const bool is_night,
                const bool is_available,
                const uint8_t price
                );

	void AddStop(const std::string_view name, const detail::Coordinates& coordinates);

	const Bus* FindBus(const std::string_view name) const;

	const Stop* FindStop(const std::string_view name) const;

	const BusInfo GetBusInfo(const Bus* current_bus) const; 

	const std::set<Bus*> GetBusesForStop(const std::string_view name) const; 

	void SetDistance(const Stop* from, const Stop* to, int distance);

	int GetDistance(const Stop* from, const Stop* to) const;

	const std::map<std::string_view, const Bus*> GetSortedBuses() const;
	[[maybe_unused]] const std::map<std::string_view, const Bus*> GetSortedBuses(const std::string& bus_name) const;

	const std::map<std::string_view, const Stop*> GetSortedStops() const;

private:
	friend class RequestHandler;

	size_t ComputeUniqueStopsCount(std::string_view bus_number) const;

	// База данных остановок: name=название_остановки, coords={широта, долгота}
	std::deque<Stop> stops_;

	// Хранит <название_остановки <название_остановки, координаты_остановки>>
	std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;

	// База данных автобусов: name - имя автобуса, stops - вектор его остановок
	std::deque<Bus> buses_;

	// Хранит <название_остановки <название_остановки, координаты_остановки>>
	std::unordered_map<std::string_view, const Bus*> busname_to_bus_;

	// Хранит название остановки и автобусы, которые посещают эту остановку
	std::unordered_map<std::string_view, std::set<Bus*>> stop_buses_;

	// расстояние между остановками { от, до }
	std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopHasher> distances_;
};
