#include "transport_catalogue.h" 
#include <fstream>

// РґРѕР±Р°РІР»РµРЅРёРµ РјР°СЂС€СЂСѓС‚Р° РІ Р±Р°Р·Сѓ,
void TransportCatalogue::AddBus(Bus bus) {
    buses_.push_back(std::move(bus));
    busname_to_bus_[buses_.back().name] = std::make_shared<Bus>(buses_.back());
	for (auto& stop : bus.stops) { 
		stop_buses_[stop->name].emplace(&buses_.back());
	} 
}

void TransportCatalogue::DeleteBus(std::shared_ptr<Bus> bus){
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
    for (auto it = buses_.begin(); it != buses_.end();) {
        if (it->name == bus->name) {
            it = buses_.erase(it);
        }
        else {
            ++it;
        }
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

void TransportCatalogue::DeleteStop(const std::shared_ptr<const Stop>& stop) {
    if (!stop) {
        return;
    }

    auto stop_name = stop->name;

    // Удаляем остановку из всех автобусов, которые проходят через неё
    if (stop_buses_.count(stop_name)) {
        for (auto& bus : stop_buses_[stop_name]) {
            auto& stops = bus->stops;  // Получаем вектор остановок для конкретного автобуса
            // Находим итератор на элементы, равные `stop`
            stops.erase(std::remove(stops.begin(), stops.end(), stop), stops.end());
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

    // Удаляем саму остановку из `stopname_to_stop_`
    stopname_to_stop_.erase(stop_name);

    
    for (auto it = stops_.begin(); it != stops_.end();) {
        if (it->name == stop_name) {
            it = stops_.erase(it); // Возвращает итератор на следующий элемент
        }
        else {
            ++it; // Переход к следующему элементу
        }
    }
}

void TransportCatalogue::UpdateStop(const QStringView old_name, const QStringView new_name, const double latitude, const double longitude)
{
    
}

// РїРѕРёСЃРє РјР°СЂС€СЂСѓС‚Р° РїРѕ РёРјРµРЅРё,
std::shared_ptr<const Bus> TransportCatalogue::FindBus(const QStringView busname) const {
    auto iter = busname_to_bus_.find(busname.toString());
	if (iter != busname_to_bus_.end()) {
		return iter->second;
	}
	else {
		return nullptr;
	}
}

// РїРѕРёСЃРє РѕСЃС‚Р°РЅРѕРІРєРё РїРѕ РёРјРµРЅРё,
std::shared_ptr<const Stop> TransportCatalogue::FindStop(const QStringView stopname) const {
    auto iter = stopname_to_stop_.find(stopname.toString());
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

const std::set<std::shared_ptr<Bus>> TransportCatalogue::GetBusesForStop(const QStringView stopname) const {
    return GetBusesForStop(stopname);
}
std::set<std::shared_ptr<Bus>> TransportCatalogue::GetBusesForStop(const QStringView stopname) {
    auto it = stop_buses_.find(stopname.toString());
    if (it != stop_buses_.end()) {
        return it->second;
    }
    return std::set<std::shared_ptr<Bus>>();
}

const std::vector<std::shared_ptr<const Stop>> TransportCatalogue::GetStopsForBus(const QStringView name) const{
    return GetStopsForBus(name);
}
std::vector<std::shared_ptr<const Stop>> TransportCatalogue::GetStopsForBus(const QStringView name) {
    auto it = busname_to_bus_.find(name.toString());
    if (it != busname_to_bus_.end()) {
        return it->second->stops;
    }
    return std::vector<std::shared_ptr<const Stop>>();
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

const std::map<QStringView, std::shared_ptr<const Bus>> TransportCatalogue::GetSortedBuses() const {
    std::map<QStringView, std::shared_ptr<const Bus>> result;
    for (const auto& bus : busname_to_bus_) {
        result.emplace(bus);
	}
    return result;
} 

[[maybe_unused]] const std::map<QStringView, std::shared_ptr<const Bus>> TransportCatalogue::GetSortedBuses(const QStringView bus_name) const {
    std::map<QStringView, std::shared_ptr<const Bus>> result;
	for (auto& [bus_name, bus_ptr] : busname_to_bus_) {
		if (bus_ptr->name == bus_name) {
			result[bus_name] = bus_ptr;
		}
		else {
            const_cast<Bus*>(bus_ptr.get())->color_index = 0;
			result[bus_name] = bus_ptr;
		}
	}
	return result;
}

const std::map<QStringView, std::shared_ptr<const Stop>> TransportCatalogue::GetSortedStops() const {
    std::map<QStringView, std::shared_ptr<const Stop>> result;
    for (const auto& [name, stop_ptr] : stopname_to_stop_) {
        result.emplace(name, stop_ptr);
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
void TransportCatalogue::UpdateBus(std::shared_ptr<Bus>& bus) {
    auto it = busname_to_bus_.find(bus->name);
    if (it != busname_to_bus_.end()) {
        // Обновляем существующую запись
        it->second = std::const_pointer_cast<const Bus>(bus); // Преобразование shared_ptr<Bus> -> shared_ptr<const Bus>

        // Обновляем информацию о остановках
        for (const auto& stop : bus->stops) {
            auto stop_it = stop_buses_.find(stop->name);
            if (stop_it == stop_buses_.end()) {
                stop_buses_[stop->name].insert(bus); // Добавляем shared_ptr<Bus> в set
            }
        }
    }
    else {
        // Если автобус не найден, добавляем как новый
        AddBus(*bus);
    }
}


void TransportCatalogue::UpdateBusnameToBus() {
    if (!busname_to_bus_.empty()){
        busname_to_bus_.clear();
    }
    for(const auto& bus : buses_){
		busname_to_bus_[bus.name] = std::make_shared<Bus>(bus);
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
                stop_buses_[stop_name].insert(std::const_pointer_cast<Bus>(bus_it->second));
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
        const Stop* from_stop = FindStop(from_stop_name).get();
        const Stop* to_stop = FindStop(to_stop_name).get();

		if (from_stop != nullptr && to_stop != nullptr) {
			SetDistance(from_stop->name, to_stop->name, distance);
		}
    }
}
