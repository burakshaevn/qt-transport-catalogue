#include "transport_catalogue.h" 
#include <fstream>

// РґРѕР±Р°РІР»РµРЅРёРµ РјР°СЂС€СЂСѓС‚Р° РІ Р±Р°Р·Сѓ,
void TransportCatalogue::AddBus(Bus& bus) {
    buses_.push_back(bus);
	busname_to_bus_[buses_.back().name] = &buses_.back();
	for (auto& stop : bus.stops) { 
		stop_buses_[stop->name].emplace(&buses_.back());
	} 
}

void TransportCatalogue::DeleteBus(Bus* bus){
    if (!bus) {
        return;
    }

    busname_to_bus_.erase(bus->name);

    for (auto& stop : bus->stops) {
        auto& buses_at_stop = stop_buses_[stop->name];
        buses_at_stop.erase(bus);
        if (buses_at_stop.empty()) {
            stop_buses_.erase(stop->name);
        }
    }

    auto it = std::find_if(buses_.begin(), buses_.end(), [&](const Bus& b) {
        return &b == bus;
    });
    if (it != buses_.end()) {
        buses_.erase(it);
    }
}

// РґРѕР±Р°РІР»РµРЅРёРµ РѕСЃС‚Р°РЅРѕРІРєРё РІ Р±Р°Р·Сѓ,
void TransportCatalogue::AddStop(const QStringView stopname, const detail::Coordinates& coordinates) {
	stops_.push_back({ stopname.toString(), coordinates});
    // stopname_to_stop_[stops_.back().name] = &stops_.back();
    stopname_to_stop_[stops_.back().name] = std::make_shared<Stop>(stops_.back());
}

void TransportCatalogue::AddStopForBus(const Stop* stop) {
    // stopname_to_stop_[stop->name] = stop;
    stopname_to_stop_[stop->name] = std::make_shared<Stop>(stops_.back());
}

void TransportCatalogue::DeleteStop(Stop* stop) {
    if (!stop) {
        return;
    }

    auto stop_name = stop->name;

    // Удаляем остановку из всех автобусов, которые проходят через неё
    if (stop_buses_.count(stop_name)) {
        for (Bus* bus : stop_buses_[stop_name]) {
            auto& stops = bus->stops;  // Получаем вектор остановок для конкретного автобуса
            // Находим итератор на элемент, равный `stop`
            auto it = std::remove(stops.begin(), stops.end(), stop);
            // Удаляем элемент
            stops.erase(it, stops.end());

        }
        // Удаляем записи об остановке из `stop_buses_`
        stop_buses_.erase(stop_name);
    }

    // Удаляем все расстояния, связанные с данной остановкой
    for (auto it = distances_.begin(); it != distances_.end(); ) {
        const auto& [from_stop, to_stop] = it->first;
        if (from_stop == stop_name || to_stop == stop_name) {
            it = distances_.erase(it);
        } else {
            ++it;
        }
    }

    // Удаляем саму остановку из `stopname_to_stop_` и `stops_`
    stopname_to_stop_.erase(stop_name);

    auto it = std::find_if(stops_.begin(), stops_.end(), [&](const Stop& s) {
        return &s == stop;
    });
    if (it != stops_.end()) {
        stops_.erase(it);
    }
}

// void TransportCatalogue::UpdateStop(const QStringView old_name, const QStringView new_name, const double latitude, const double longitude) {
//     auto it = stopname_to_stop_.find(old_name.toString());
//     if (it != stopname_to_stop_.end()) {
//         // Копируем старую остановку
//         auto new_stop_ptr = std::make_shared<Stop>(*it->second);

//         // Обновляем поля
//         new_stop_ptr->name = new_name.toString();
//         new_stop_ptr->coords.lat = latitude;
//         new_stop_ptr->coords.lng = longitude;

//         // Заменяем везде старый объект новым
//         stopname_to_stop_.erase(it);
//         stopname_to_stop_[new_name.toString()] = new_stop_ptr;

//         // Обновляем все маршруты (Bus), которые содержат старую остановку
//         for (auto& bus : buses_) {
//             for (auto& stop : bus.stops) {
//                 if (stop == it->second.get()) {
//                     stop = new_stop_ptr.get(); // Обновляем указатель на новую версию остановки
//                 }
//             }
//         }
//     }
// }

// РїРѕРёСЃРє РјР°СЂС€СЂСѓС‚Р° РїРѕ РёРјРµРЅРё,
const Bus* TransportCatalogue::FindBus(const QStringView busname) const {
    auto iter = busname_to_bus_.find(busname.toString());
	if (iter != busname_to_bus_.end()) {
		return iter->second;
	}
	else {
		return nullptr;
	}
}

// РїРѕРёСЃРє РѕСЃС‚Р°РЅРѕРІРєРё РїРѕ РёРјРµРЅРё,
const Stop* TransportCatalogue::FindStop(const QStringView stopname) const {
    auto iter = stopname_to_stop_.find(stopname.toString());
	if (iter != stopname_to_stop_.end()) {
        return iter->second.get();
	}
	else {
		return nullptr;
	}
} 

const BusInfo TransportCatalogue::GetBusInfo(const Bus* current_bus) const {
	BusInfo bus_info;
    bus_info.name = current_bus->name;
	// РџРѕРґСЃС‡С‘С‚ РєРѕР»РёС‡РµСЃС‚РІР° РѕСЃС‚Р°РІРЅРѕРІРѕРє stop_size
	bus_info.count_stops = current_bus->stops.size();

	// РџРѕРґСЃС‡С‘С‚ СѓРЅРёРєР°Р»СЊРЅС‹С… РѕСЃС‚Р°РЅРѕРІРѕРє 
	std::unordered_set<QString> unique_stops;
	for (const auto& stop : current_bus->stops) {
		unique_stops.insert(stop->name);
	}
	bus_info.unique_count_stops = unique_stops.size();

	// Р”Р»РёРЅР° РјР°СЂС€СЂСѓС‚Р° РІ РјРµС‚СЂР°С…
	int route_length = 0;
	double geographic_length = 0.0;

	if (current_bus->stops.size() != 0) {
		for (size_t i = 0; i < current_bus->stops.size() - 1; ++i) {
			auto from = current_bus->stops[i];
			auto to = current_bus->stops[i + 1];
			route_length += GetDistance(from->name, to->name);
			geographic_length += detail::ComputeDistance(from->coords, to->coords);
		}
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

const std::set<Bus*> TransportCatalogue::GetBusesForStop(const QStringView stopname) const {
 //    auto it = stop_buses_.find(stopname.toString());
    // if (it != stop_buses_.end()) {
    // 	return it->second;
    // }
    // return std::set<Bus*>();
    return GetBusesForStop(stopname);
}
std::set<Bus*> TransportCatalogue::GetBusesForStop(const QStringView stopname) {
    auto it = stop_buses_.find(stopname.toString());
    if (it != stop_buses_.end()) {
        return it->second;
    }
    return std::set<Bus*>();        
}

const std::vector<const Stop*> TransportCatalogue::GetStopsForBus(const QStringView name) const{
    return GetStopsForBus(name);
}
std::vector<const Stop*> TransportCatalogue::GetStopsForBus(const QStringView name) {
    auto it = busname_to_bus_.find(name.toString());
    if (it != busname_to_bus_.end()) {
        return it->second->stops;
    }
    return std::vector<const Stop*>();
}

void TransportCatalogue::SetDistance(const QStringView from, const QStringView to, const int distance) {
    distances_[{from.toString(), to.toString()}] = distance;
} 
int TransportCatalogue::GetDistance(const QStringView from, const QStringView to) const {
    if (auto it = distances_.find({ from.toString(), to.toString() }); it != distances_.end()) {
		return it->second;
	}
    if (auto it = distances_.find({ to.toString(), from.toString() }); it != distances_.end()) {
		return it->second;
	}
	return 0;  
}

size_t TransportCatalogue::ComputeUniqueStopsCount(QStringView bus_number) const {
	std::unordered_set<QStringView> unique_stops;
    for (const auto& stop : busname_to_bus_.at(bus_number.toString())->stops) {
		unique_stops.insert(stop->name);
	}
	return unique_stops.size();
}

const std::map<QStringView, const Bus*> TransportCatalogue::GetSortedBuses() const {
	std::map<QStringView, const Bus*> result;
	for (const auto& bus : busname_to_bus_) {
		result.emplace(bus);
	}
	return result;
} 

[[maybe_unused]] const std::map<QStringView, const Bus*> TransportCatalogue::GetSortedBuses(const QStringView bus_name) const {
	std::map<QStringView, const Bus*> result;
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

// const std::map<QStringView, const Stop*> TransportCatalogue::GetSortedStops() const {
// 	std::map<QStringView, const Stop*> result;
// 	for (const auto& stop : stopname_to_stop_) {
// 		result.emplace(stop);
// 	}
// 	return result;
// }
const std::map<QStringView, const Stop*> TransportCatalogue::GetSortedStops() const {
    std::map<QStringView, const Stop*> result;
    for (const auto& [name, stop_ptr] : stopname_to_stop_) {
        result.emplace(name, stop_ptr.get());
    }
    return result;
}

double TransportCatalogue::ComputeTfIdfForBus(const Bus* bus,
                                               const std::optional<QStringView> name,
                                               const std::optional<QStringView> desired_stop,
                                               const std::optional<bool> is_roundtrip,
                                               const std::optional<std::set<BusType>>& bus_types,
                                               const std::optional<uint8_t> capacity,
                                               const std::optional<bool> has_wifi,
                                               const std::optional<bool> has_sockets,
                                               const std::optional<bool> is_night,
                                               const std::optional<bool> is_day,
                                               const std::optional<bool> is_available,
                                               const std::optional<uint8_t> price
                                               ){
    int count_term_in_bus = 0;
    int total_terms = 0;

    if (name.has_value()) {
        ++total_terms;
        if (bus->name == name.value()) {
            ++count_term_in_bus;
        }
    }
    if (desired_stop.has_value() && !desired_stop->empty()) {
        ++total_terms;
        for (const auto& stop : bus->stops) {
            if (stop->name == desired_stop->toString()) {
                ++count_term_in_bus;
                break;
            }
        }
    }
    if (capacity.has_value()) {
        ++total_terms;
        if (bus->capacity == capacity) {
            ++count_term_in_bus;
        }
    }
    if (price.has_value()) {
        ++total_terms;
        if (bus->price == price) {
            ++count_term_in_bus;
        }
    }
    if (has_wifi.has_value()) {
        ++total_terms;
        if (bus->has_wifi == has_wifi) {
            ++count_term_in_bus;
        }
    }
    if (has_sockets.has_value()) {
        ++total_terms;
        if (bus->has_sockets == has_sockets) {
            ++count_term_in_bus;
        }
    }
    if (is_night.has_value()) {
        ++total_terms;
        if (bus->is_night == is_night) {
            ++count_term_in_bus;
        }
    }
    if (is_day.has_value()) {
        ++total_terms;
        if (bus->is_day == is_day) {
            ++count_term_in_bus;
        }
    }
    if (is_roundtrip.has_value()) {
        ++total_terms;
        if (bus->is_roundtrip == is_roundtrip) {
            ++count_term_in_bus;
        }
    }
    if (is_available.has_value()) {
        ++total_terms;
        if (bus->is_available == is_available) {
            ++count_term_in_bus;
        }
    }
    if (bus_types.has_value()) {
        ++total_terms;
        if (bus_types.value().find(bus->bus_type) != bus_types.value().end()) {
            ++count_term_in_bus;
        }
    }
    if (total_terms == 0) {
        return 0.0;
    }
    return (static_cast<double>(count_term_in_bus) / total_terms) * 1.;
}

// Signals

void TransportCatalogue::UpdateStops() {
    if (!stops_.empty()){
        stops_.clear();
    }
    QSqlQuery query = db_manager_.ExecuteSelectQuery(
		QString("SELECT * FROM stops;")
	);
	while (query.next()) {
		Stop stop;
		stop.name = query.value("name").toString();
		stop.coords.lat = query.value("latitude").toDouble();
		stop.coords.lng = query.value("longitude").toDouble();
		stops_.push_back(stop);
	}
}
void TransportCatalogue::UpdateStopnameToStop() {
	if (!stopname_to_stop_.empty()) {
		stopname_to_stop_.clear();
	}
    for (const auto& stop : stops_) {
        stopname_to_stop_[stop.name] = std::make_shared<Stop>(stop);
	}
}
void TransportCatalogue::UpdateBuses() {
    if (!buses_.empty()){
        buses_.clear();
    }
	QSqlQuery query = db_manager_.ExecuteSelectQuery(
		QString("SELECT * FROM buses;")
	);
	while (query.next()) {
		Bus bus;
		bus.name = query.value("name").toString();
		bus.is_roundtrip = query.value("is_roundtrip").toBool();
		bus.color_index = query.value("color_index").toUInt();
		bus.bus_type = StringToBusType(query.value("bus_type").toString());
		bus.capacity = query.value("capacity").toUInt();
		bus.has_wifi = query.value("has_wifi").toBool();
		bus.has_sockets = query.value("has_sockets").toBool();
		bus.is_night = query.value("is_night").toBool();
		bus.is_day = query.value("is_day").toBool();
		bus.is_available = query.value("is_available").toBool();
		bus.price = query.value("price").toUInt();
		QSqlQuery stops_query = db_manager_.ExecuteSelectQuery(
			QString("SELECT s.name, s.latitude, s.longitude FROM bus_stops bs LEFT JOIN stops s ON bs.stop_id = s.id WHERE bs.bus_id = %1;").arg(query.value("id").toInt())
		);
		static std::vector<std::unique_ptr<Stop>> stops_storage;
		while (stops_query.next()) {
			auto stop_ptr = std::make_unique<Stop>();
			stop_ptr->name = stops_query.value("name").toString();
			stop_ptr->coords.lat = stops_query.value("latitude").toDouble();
			stop_ptr->coords.lng = stops_query.value("longitude").toDouble();

			bus.stops.emplace_back(stop_ptr.get());
			stops_storage.emplace_back(std::move(stop_ptr));
		}
		buses_.push_back(bus);
	}
}
void TransportCatalogue::UpdateBus(const Bus* bus) {
    auto it = busname_to_bus_.find(bus->name);
	if (it != busname_to_bus_.end()) {
        it->second = bus;

        for(auto& stop : bus->stops){
            auto it = stop_buses_.find(stop->name);
            if (it == stop_buses_.end()){
                stop_buses_[stop->name].emplace(const_cast<Bus*>(bus));
            }
        }
    }
	else {
        // Если автобус не найден, добавляем как новый
		AddBus(const_cast<Bus&>(*bus));
    }
}
void TransportCatalogue::UpdateBusnameToBus() {
    if (!busname_to_bus_.empty()){
        busname_to_bus_.clear();
    }
    for(const auto& bus : buses_){
		busname_to_bus_[bus.name] = &bus;
	}
}
void TransportCatalogue::UpdateStopBuses() {
    if (!stop_buses_.empty()){
        stop_buses_.clear();
    }
    for (size_t i = 1; i <= db_manager_.GetRowsCount(QString("buses")); ++i) {
		QSqlQuery stops_query = db_manager_.ExecuteSelectQuery(
			QString("SELECT b.name AS bus_name, s.name AS stop_name FROM buses b "
				"JOIN bus_stops bs ON b.id = bs.bus_id "
				"JOIN stops s ON bs.stop_id = s.id WHERE b.id = %1;").arg(i)
		);
		 
		while (stops_query.next()) {
			QString bus_name = stops_query.value(0).toString();
			QString stop_name = stops_query.value(1).toString();
			 
			auto bus_it = busname_to_bus_.find(bus_name);
			if (bus_it != busname_to_bus_.end()) {
				Bus* bus_ptr = const_cast<Bus*>(bus_it->second);
				stop_buses_[stop_name].insert(bus_ptr);
			}
		}
	}
}
void TransportCatalogue::UpdateDistances() {
    if (!distances_.empty()) {
        distances_.clear();
    }
    QSqlQuery query = db_manager_.ExecuteSelectQuery("SELECT d.id, s1.name AS from_stop_name, s2.name AS to_stop_name, d.distance FROM distances d JOIN stops s1 ON d.from_stop_id = s1.id JOIN stops s2 ON d.to_stop_id = s2.id;");
	while (query.next()) {
		QString from_stop_name = query.value("from_stop_name").toString();
		QString to_stop_name = query.value("to_stop_name").toString();
		int distance = query.value("distance").toInt();
		const Stop* from_stop = FindStop(from_stop_name);
		const Stop* to_stop = FindStop(to_stop_name);

		if (from_stop != nullptr && to_stop != nullptr) {
			SetDistance(from_stop->name, to_stop->name, distance);
		}
    }
}
