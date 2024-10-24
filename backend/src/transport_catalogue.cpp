//#include "transport_catalogue.h" 
//
//// добавление маршрута в базу,
//void TransportCatalogue::AddBus(const std::string_view busname,
//                                std::vector<const Stop*>& stops,
//                                const bool is_roundtrip,
//                                const size_t color_index,
//                                const std::array<uint8_t, 3> rgb,
//                                const BusType bus_type,
//                                const uint8_t capacity,
//                                const bool has_wifi,
//                                const bool has_sockets,
//                                const bool is_night,
//                                const bool is_available,
//                                const uint8_t price
//                                ) {
//    buses_.push_back({ std::string(busname), stops, is_roundtrip, color_index, rgb, bus_type, capacity, has_wifi, has_sockets, is_night, is_available, price });
//	busname_to_bus_[buses_.back().name] = &buses_.back();
//	for (auto& stop : stops) { 
//		stop_buses_[stop->name].emplace(&buses_.back());
//	} 
//}
//
//// добавление остановки в базу,
//void TransportCatalogue::AddStop(const std::string_view stopname, const detail::Coordinates& coordinates) {
//	stops_.push_back({ std::string(stopname), coordinates });
//	stopname_to_stop_[stops_.back().name] = &stops_.back();
//} 
//
//// поиск маршрута по имени,
//const Bus* TransportCatalogue::FindBus(const std::string_view busname) const {
//	auto iter = busname_to_bus_.find(busname);
//	if (iter != busname_to_bus_.end()) {
//		return iter->second;
//	}
//	else {
//		return nullptr;
//	}
//}
//
//// поиск остановки по имени,
//const Stop* TransportCatalogue::FindStop(const std::string_view stopname) const {
//	auto iter = stopname_to_stop_.find(stopname);
//	if (iter != stopname_to_stop_.end()) {
//		return iter->second;
//	}
//	else {
//		return nullptr;
//	}
//} 
//
//const BusInfo TransportCatalogue::GetBusInfo(const Bus* current_bus) const {
//	BusInfo bus_info;
//    bus_info.name = current_bus->name;
//	// Подсчёт количества оставновок stop_size
//	bus_info.count_stops = current_bus->stops.size();
//
//	// Подсчёт уникальных остановок 
//	std::unordered_set<std::string> unique_stops;
//	for (const auto& stop : current_bus->stops) {
//		unique_stops.insert(stop->name);
//	}
//	bus_info.unique_count_stops = unique_stops.size();
//
//	// Длина маршрута в метрах
//	int route_length = 0;
//	double geographic_length = 0.0;
//
//	for (size_t i = 0; i < current_bus->stops.size() - 1; ++i) {
//		auto from = current_bus->stops[i];
//		auto to = current_bus->stops[i + 1]; 
//		route_length += GetDistance(from, to);
//		geographic_length += detail::ComputeDistance(from->coords, to->coords); 
//	}
//	 
//	bus_info.len = route_length;
//	bus_info.curvature = route_length / geographic_length;
//
//    bus_info.rgb = current_bus->rgb;
//    bus_info.has_sockets = current_bus->has_sockets;
//    bus_info.has_wifi = current_bus->has_wifi;
//    bus_info.bus_type = current_bus->bus_type;
//    bus_info.capacity = current_bus->capacity;
//    bus_info.is_night = current_bus->is_night;
//
//    bus_info.is_available = current_bus->is_available;
//    bus_info.price = current_bus->price;
//
//	return bus_info;
//}
//
//const std::set<Bus*> TransportCatalogue::GetBusesForStop(const std::string_view stopname) const {
//	auto it = stop_buses_.find(stopname);
//	if (it != stop_buses_.end()) {
//		return it->second;
//	}
//	return std::set<Bus*>();
//}
//
//void TransportCatalogue::SetDistance(const Stop* from, const Stop* to, int distance) {
//	distances_[{from, to}] = distance;
//} 
//
//int TransportCatalogue::GetDistance(const Stop* from, const Stop* to) const {
//	if (auto it = distances_.find({ from, to }); it != distances_.end()) {
//		return it->second;
//	}
//	// Проверка на обратное направление, если расстояние не найдено в прямом направлении
//	if (auto it = distances_.find({ to, from }); it != distances_.end()) {
//		return it->second;
//	}
//	return 0;  
//}
//
//size_t TransportCatalogue::ComputeUniqueStopsCount(std::string_view bus_number) const {
//	std::unordered_set<std::string_view> unique_stops;
//	for (const auto& stop : busname_to_bus_.at(bus_number)->stops) {
//		unique_stops.insert(stop->name);
//	}
//	return unique_stops.size();
//}
//
//const std::map<std::string_view, const Bus*> TransportCatalogue::GetSortedBuses() const {
//	std::map<std::string_view, const Bus*> result;
//	for (const auto& bus : busname_to_bus_) {
//		result.emplace(bus);
//	}
//	return result;
//}
//
//[[maybe_unused]] const std::map<std::string_view, const Bus*> TransportCatalogue::GetSortedBuses(const std::string& bus_name) const {
//	std::map<std::string_view, const Bus*> result;
//	for (const auto& bus : busname_to_bus_) {
//		if (bus.second->name == bus_name) {
//			result.emplace(bus);
//		}
//		else {
//			const_cast<Bus*>(bus.second)->color_index = 0;
//			result.emplace(bus);
//		}
//	}
//	return result;
//}
//
//const std::map<std::string_view, const Stop*> TransportCatalogue::GetSortedStops() const {
//	std::map<std::string_view, const Stop*> result;
//	for (const auto& stop : stopname_to_stop_) {
//		result.emplace(stop);
//	}
//	return result;
//}
#include "transport_catalogue.h"

// Преобразование типа автобуса в строку
std::string TransportCatalogue::BusTypeToString(BusType bus_type) {
    switch (bus_type) {
    case BusType::Autobus:
        return "Autobus";
    case BusType::Electrobus:
        return "Electrobus";
    case BusType::Trolleybus:
        return "Trolleybus";
    default:
        return "Undefined";
    }
}

// Преобразование строки в тип автобуса
BusType TransportCatalogue::StringToBusType(const std::string& bus_type) const {
    if (bus_type == "Autobus") {
        return BusType::Autobus;
    }
    else if (bus_type == "Electrobus") {
        return BusType::Electrobus;
    }
    else if (bus_type == "Trolleybus") {
        return BusType::Trolleybus;
    }
    else {
        return BusType::Undefined;
    }
}

// Добавление автобуса в базу данных
void TransportCatalogue::AddBus(const std::string_view name, std::vector<const Stop*>& stops, const bool is_roundtrip,
    const size_t color_index, const std::array<uint8_t, 3> rgb, const BusType bus_type,
    const uint8_t capacity, const bool has_wifi, const bool has_sockets,
    const bool is_night, const bool is_available, const uint8_t price) {

    QString bus_type_str = QString::fromStdString(BusTypeToString(bus_type));

    QStringList stop_names;
    for (const Stop* stop : stops) {
        stop_names << QString::fromStdString(stop->name);
    }

    db_manager_.AddBus(
        QString::fromStdString(std::string(name)),
        stop_names,
        is_roundtrip,
        color_index,
        bus_type_str,
        capacity,
        has_wifi,
        has_sockets,
        is_night,
        is_available,
        price
    );
}

// Добавление остановки в базу данных
void TransportCatalogue::AddStop(const std::string_view name, const detail::Coordinates& coordinates) {
    db_manager_.AddStop(QString::fromStdString(std::string(name)), coordinates.lat, coordinates.lng);
}

// Установка расстояния между остановками
void TransportCatalogue::SetDistance(const Stop* from, const Stop* to, int distance) {
    db_manager_.AddDistance(QString::fromStdString(from->name), QString::fromStdString(to->name), distance);
}

// Поиск автобуса по имени
std::optional<Bus> TransportCatalogue::FindBus(const std::string_view name) const {
    QSqlQuery query = db_manager_.executeSelectQuery(
        QString("SELECT * FROM buses WHERE name = '%1';").arg(QString::fromStdString(std::string(name)))
    );

    if (query.next()) {
        Bus bus;
        bus.name = query.value("name").toString().toStdString();
        bus.is_roundtrip = query.value("is_roundtrip").toBool();
        bus.color_index = query.value("color_index").toUInt();
        bus.bus_type = StringToBusType(query.value("bus_type").toString().toStdString());
        bus.capacity = query.value("capacity").toUInt();
        bus.has_wifi = query.value("has_wifi").toBool();
        bus.has_sockets = query.value("has_sockets").toBool();
        bus.is_night = query.value("is_night").toBool();
        bus.is_available = query.value("is_available").toBool();
        bus.price = query.value("price").toUInt();

        QSqlQuery stops_query = db_manager_.executeSelectQuery(
            QString("SELECT s.name, s.latitude, s.longitude FROM bus_stops bs JOIN stops s ON bs.stop_id = s.id WHERE bs.bus_id = %1;").arg(query.value("id").toInt())
        );

        static std::vector<std::unique_ptr<Stop>> stops_storage;
        while (stops_query.next()) {
            // Создаем объект Stop в динамической памяти
            auto stop_ptr = std::make_unique<Stop>();
            stop_ptr->name = stops_query.value("name").toString().toStdString();
            stop_ptr->coords.lat = stops_query.value("latitude").toDouble();
            stop_ptr->coords.lng = stops_query.value("longitude").toDouble();

            // Добавляем указатель на этот объект в вектор bus.stops
            bus.stops.emplace_back(stop_ptr.get());
            stops_storage.emplace_back(std::move(stop_ptr));
        }
        return bus;
    }
    return std::nullopt;
}

// Поиск остановки по имени
std::optional<Stop> TransportCatalogue::FindStop(const std::string_view name) const {
    QSqlQuery query = db_manager_.executeSelectQuery(
        QString("SELECT * FROM stops WHERE name = '%1'").arg(QString::fromStdString(std::string(name)))
    );

    if (query.next()) {
        Stop stop;
        stop.name = query.value("name").toString().toStdString();
        stop.coords.lat = query.value("latitude").toDouble();
        stop.coords.lng = query.value("longitude").toDouble();
        return stop;
    }

    return std::nullopt;
}

// Получение информации о маршруте автобуса
BusInfo TransportCatalogue::GetBusInfo(const std::string_view bus_name) const {
    auto bus_opt = FindBus(bus_name);
    if (!bus_opt) {
        throw std::runtime_error("Bus not found");
    }

    const Bus& bus = *bus_opt;
    BusInfo bus_info;
    bus_info.name = bus.name;
    bus_info.count_stops = bus.stops.size();

    // Подсчёт уникальных остановок
    std::unordered_set<std::string> unique_stops;
    for (const auto& stop : bus.stops) {
        unique_stops.insert(stop->name);
    }
    bus_info.unique_count_stops = unique_stops.size();

    // Подсчёт длины маршрута
    int route_length = 0;
    double geographic_length = 0.0;

    if (bus.stops.size() != 0) {
        for (size_t i = 0; i < bus.stops.size() - 1; ++i) {
            const Stop* from = bus.stops[i];
            const Stop* to = bus.stops[i + 1];
            route_length += GetDistance(from, to);
            geographic_length += detail::ComputeDistance(from->coords, to->coords);
        }
    }

    bus_info.len = route_length;
    bus_info.curvature = route_length / geographic_length;

    bus_info.rgb = bus.rgb;
    bus_info.has_sockets = bus.has_sockets;
    bus_info.has_wifi = bus.has_wifi;
    bus_info.bus_type = bus.bus_type;
    bus_info.capacity = bus.capacity;
    bus_info.is_night = bus.is_night;
    bus_info.is_available = bus.is_available;
    bus_info.price = bus.price;

    return bus_info;
}

// Получение расстояния между двумя остановками
int TransportCatalogue::GetDistance(const Stop* from, const Stop* to) const {
    QSqlQuery query = db_manager_.executeSelectQuery(
        QString("SELECT distance FROM distances WHERE from_stop = '%1' AND to_stop = '%2'")
        .arg(QString::fromStdString(from->name))
        .arg(QString::fromStdString(to->name))
    );

    if (query.next()) {
        return query.value("distance").toInt();
    }
    
    return 0;
}

std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopHasher> TransportCatalogue::GetDistances() {
    std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopHasher> distances;

    QSqlQuery query = db_manager_.executeSelectQuery("SELECT from_stop, to_stop, distance FROM distances;");
    while (query.next()) {
        std::string from_stop_name = query.value("from_stop").toString().toStdString();
        std::string to_stop_name = query.value("to_stop").toString().toStdString();
        int distance = query.value("distance").toInt();

        // Находим указатели на остановки
        auto from_stop_opt = FindStop(from_stop_name);
        auto to_stop_opt = FindStop(to_stop_name);

        if (from_stop_opt.has_value() && to_stop_opt.has_value()) {
            const Stop* from_stop = &from_stop_opt.value();
            const Stop* to_stop = &to_stop_opt.value();
            distances[{from_stop, to_stop}] = distance;
        }
    }
    return distances;
}

// Получение всех автобусов, отсортированных по имени 
std::map<std::string, Bus> TransportCatalogue::GetSortedBuses() const {
    std::map<std::string, Bus> result; 
    static std::vector<std::unique_ptr<Stop>> temp_storage; 

    if (!db_manager_.Open()) {
        qDebug() << "Не удалось подключиться к базе данных.";
        return result;
    }

    QSqlQuery query = db_manager_.executeSelectQuery("SELECT * FROM buses;");

    while (query.next()) {
        Bus bus;
        bus.name = query.value("name").toString().toStdString();
        bus.is_roundtrip = query.value("is_roundtrip").toBool();
        bus.color_index = query.value("color_index").toInt();
        /*bus.rgb = {
            static_cast<uint8_t>(query.value("rgb_r").toInt()),
            static_cast<uint8_t>(query.value("rgb_g").toInt()),
            static_cast<uint8_t>(query.value("rgb_b").toInt())
        };*/
        bus.bus_type = StringToBusType(query.value("bus_type").toString().toStdString());
        bus.capacity = static_cast<uint8_t>(query.value("capacity").toInt());
        bus.has_wifi = query.value("has_wifi").toBool();
        bus.has_sockets = query.value("has_sockets").toBool();
        bus.is_night = query.value("is_night").toBool();
        bus.is_available = query.value("is_available").toBool();
        bus.price = static_cast<uint8_t>(query.value("price").toInt());
         
        QSqlQuery stops_query = db_manager_.executeSelectQuery(
            QString("SELECT s.name, s.latitude, s.longitude FROM bus_stops bs JOIN stops s ON bs.stop_id = s.id WHERE bs.bus_id = %1;").arg(query.value("id").toInt())
        ); 

        while (stops_query.next()) {
            // Создаем объект Stop в динамической памяти
            auto stop_ptr = std::make_unique<Stop>();
            stop_ptr->name = stops_query.value("name").toString().toStdString();
            stop_ptr->coords.lat = stops_query.value("latitude").toDouble();
            stop_ptr->coords.lng = stops_query.value("longitude").toDouble();

            // Добавляем указатель на этот объект в вектор bus.stops
            bus.stops.emplace_back(stop_ptr.get());
            temp_storage.emplace_back(std::move(stop_ptr));
        }
         
        result[bus.name] = std::move(bus); 
    } 
    return result;
}
 
std::map<std::string, Bus> TransportCatalogue::GetSortedBuses(const std::string_view bus_name) const {
    std::map<std::string, Bus> result;
     
    QSqlQuery query;
    query.prepare("SELECT * FROM buses WHERE name = :bus_name");
    query.bindValue(":bus_name", QString::fromStdString(std::string(bus_name)));   

    if (query.exec()) {
        while (query.next()) {
            auto bus_opt = FindBus(query.value("name").toString().toStdString());
            if (bus_opt) {
                result[bus_opt->name] = *bus_opt;
            }
        }
    }
    else {
        qDebug() << "Ошибка выполнения запроса: " << query.lastError().text();
    }

    return result;
}

// Получение всех остановок, отсортированных по имени
std::map<std::string, Stop> TransportCatalogue::GetSortedStops() const {
    std::map<std::string, Stop> result;

    QSqlQuery query = db_manager_.executeSelectQuery("SELECT * FROM stops;");
    while (query.next()) {
        Stop stop;
        stop.name = query.value("name").toString().toStdString();
        stop.coords.lat = query.value("latitude").toDouble();
        stop.coords.lng = query.value("longitude").toDouble();
        result[stop.name] = stop;
    }

    return result;
}

size_t TransportCatalogue::ComputeUniqueStopsCount(const std::string_view bus_number) const {
    auto bus_opt = FindBus(bus_number);
    if (!bus_opt) {
        throw std::runtime_error("Bus not found");
    }

    const Bus& bus = *bus_opt;
    std::unordered_set<std::string> unique_stops;
    for (const auto& stop : bus.stops) {
        unique_stops.insert(stop->name);
    }
    return unique_stops.size();
}

std::set<const Bus*> TransportCatalogue::GetBusesForStop(const std::string_view stop_name) const {
    std::set<const Bus*> buses_for_stop;

    QSqlQuery query;
    query.prepare("SELECT bus_name FROM bus_stops WHERE stop_name = :stop_name");
    query.bindValue(":stop_name", QString::fromStdString(std::string(stop_name)));

    if (query.exec()) {
        while (query.next()) {
            auto bus_opt = FindBus(query.value("bus_name").toString().toStdString());
            if (bus_opt) {
                buses_for_stop.insert(&(*bus_opt));
            }
        }
    }

    return buses_for_stop;
}
