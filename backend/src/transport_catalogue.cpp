#include "transport_catalogue.h" 

// добавление маршрута в базу,
void TransportCatalogue::AddBus(const std::string_view busname,
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
                                ) {
    buses_.push_back({ std::string(busname), stops, is_roundtrip, color_index, rgb, bus_type, capacity, has_wifi, has_sockets, is_night, is_available, price });
	busname_to_bus_[buses_.back().name] = &buses_.back();
	for (auto& stop : stops) { 
		stop_buses_[stop->name].emplace(&buses_.back());
	} 
}

// добавление остановки в базу,
void TransportCatalogue::AddStop(const std::string_view stopname, const detail::Coordinates& coordinates) {
	stops_.push_back({ std::string(stopname), coordinates });
	stopname_to_stop_[stops_.back().name] = &stops_.back();
} 

// поиск маршрута по имени,
const Bus* TransportCatalogue::FindBus(const std::string_view busname) const {
	auto iter = busname_to_bus_.find(busname);
	if (iter != busname_to_bus_.end()) {
		return iter->second;
	}
	else {
		return nullptr;
	}
}

// поиск остановки по имени,
const Stop* TransportCatalogue::FindStop(const std::string_view stopname) const {
	auto iter = stopname_to_stop_.find(stopname);
	if (iter != stopname_to_stop_.end()) {
		return iter->second;
	}
	else {
		return nullptr;
	}
} 

const BusInfo TransportCatalogue::GetBusInfo(const Bus* current_bus) const {
	BusInfo bus_info;
    bus_info.name = current_bus->name;
	// Подсчёт количества оставновок stop_size
	bus_info.count_stops = current_bus->stops.size();

	// Подсчёт уникальных остановок 
	std::unordered_set<std::string> unique_stops;
	for (const auto& stop : current_bus->stops) {
		unique_stops.insert(stop->name);
	}
	bus_info.unique_count_stops = unique_stops.size();

	// Длина маршрута в метрах
	int route_length = 0;
	double geographic_length = 0.0;

	for (size_t i = 0; i < current_bus->stops.size() - 1; ++i) {
		auto from = current_bus->stops[i];
		auto to = current_bus->stops[i + 1]; 
		route_length += GetDistance(from, to);
		geographic_length += detail::ComputeDistance(from->coords, to->coords); 
	}
	 
	bus_info.len = route_length;
	bus_info.curvature = route_length / geographic_length;

    bus_info.rgb = current_bus->rgb;
    bus_info.has_sockets = current_bus->has_sockets;
    bus_info.has_wifi = current_bus->has_wifi;
    bus_info.bus_type = current_bus->bus_type;
    bus_info.capacity = current_bus->capacity;
    bus_info.is_night = current_bus->is_night;

    bus_info.is_available = current_bus->is_available;
    bus_info.price = current_bus->price;

	return bus_info;
}

const std::set<Bus*> TransportCatalogue::GetBusesForStop(const std::string_view stopname) const {
	auto it = stop_buses_.find(stopname);
	if (it != stop_buses_.end()) {
		return it->second;
	}
	return std::set<Bus*>();
}

void TransportCatalogue::SetDistance(const Stop* from, const Stop* to, int distance) {
	distances_[{from, to}] = distance;
} 

int TransportCatalogue::GetDistance(const Stop* from, const Stop* to) const {
	if (auto it = distances_.find({ from, to }); it != distances_.end()) {
		return it->second;
	}
	// Проверка на обратное направление, если расстояние не найдено в прямом направлении
	if (auto it = distances_.find({ to, from }); it != distances_.end()) {
		return it->second;
	}
	return 0;  
}

size_t TransportCatalogue::ComputeUniqueStopsCount(std::string_view bus_number) const {
	std::unordered_set<std::string_view> unique_stops;
	for (const auto& stop : busname_to_bus_.at(bus_number)->stops) {
		unique_stops.insert(stop->name);
	}
	return unique_stops.size();
}

const std::map<std::string_view, const Bus*> TransportCatalogue::GetSortedBuses() const {
	std::map<std::string_view, const Bus*> result;
	for (const auto& bus : busname_to_bus_) {
		result.emplace(bus);
	}
	return result;
}

[[maybe_unused]] const std::map<std::string_view, const Bus*> TransportCatalogue::GetSortedBuses(const std::string& bus_name) const {
	std::map<std::string_view, const Bus*> result;
	for (const auto& bus : busname_to_bus_) {
		if (bus.second->name == bus_name) {
			result.emplace(bus);
		}
		else {
			const_cast<Bus*>(bus.second)->color_index = 0;
			result.emplace(bus);
		}
	}
	return result;
}

const std::map<std::string_view, const Stop*> TransportCatalogue::GetSortedStops() const {
	std::map<std::string_view, const Stop*> result;
	for (const auto& stop : stopname_to_stop_) {
		result.emplace(stop);
	}
	return result;
}
