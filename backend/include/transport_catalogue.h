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

// РєР»Р°СЃСЃ С‚СЂР°РЅСЃРїРѕСЂС‚РЅРѕРіРѕ СЃРїСЂР°РІРѕС‡РЅРёРєР° 

struct StopHasher {
	size_t operator()(const std::pair<const Stop*, const Stop*>& points) const {
		size_t hash_first = std::hash<const void*>{}(points.first);
		size_t hash_second = std::hash<const void*>{}(points.second);
		return hash_first + hash_second * 37;
	}
};
//
//class TransportCatalogue {
//public:
//    void AddBus(const std::string_view name,
//                std::vector<const Stop*>& stops,
//                const bool is_roundtrip,
//                const size_t color_index,
//                const std::array<uint8_t, 3> rgb,
//                const BusType bus_type,
//                const uint8_t capacity,
//                const bool has_wifi,
//                const bool has_sockets,
//                const bool is_night,
//                const bool is_available,
//                const uint8_t price
//                );
//
//	void AddStop(const std::string_view name, const detail::Coordinates& coordinates);
//
//	const Bus* FindBus(const std::string_view name) const;
//
//	const Stop* FindStop(const std::string_view name) const;
//
//	const BusInfo GetBusInfo(const Bus* current_bus) const; 
//
//	const std::set<Bus*> GetBusesForStop(const std::string_view name) const; 
//
//	void SetDistance(const Stop* from, const Stop* to, int distance);
//
//	int GetDistance(const Stop* from, const Stop* to) const;
//
//	const std::map<std::string_view, const Bus*> GetSortedBuses() const;
//	[[maybe_unused]] const std::map<std::string_view, const Bus*> GetSortedBuses(const std::string& bus_name) const;
//
//	const std::map<std::string_view, const Stop*> GetSortedStops() const;
//
//	std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopHasher> GetDistances() {
//		return distances_;
//	}
//
//private:
//	friend class RequestHandler;
//
//	size_t ComputeUniqueStopsCount(std::string_view bus_number) const;
//
//	// Р‘Р°Р·Р° РґР°РЅРЅС‹С… РѕСЃС‚Р°РЅРѕРІРѕРє: name=РЅР°Р·РІР°РЅРёРµ_РѕСЃС‚Р°РЅРѕРІРєРё, coords={С€РёСЂРѕС‚Р°, РґРѕР»РіРѕС‚Р°}
//	std::deque<Stop> stops_;
//
//	// РҐСЂР°РЅРёС‚ <РЅР°Р·РІР°РЅРёРµ_РѕСЃС‚Р°РЅРѕРІРєРё <РЅР°Р·РІР°РЅРёРµ_РѕСЃС‚Р°РЅРѕРІРєРё, РєРѕРѕСЂРґРёРЅР°С‚С‹_РѕСЃС‚Р°РЅРѕРІРєРё>>
//	std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;
//
//	// Р‘Р°Р·Р° РґР°РЅРЅС‹С… Р°РІС‚РѕР±СѓСЃРѕРІ: name - РёРјСЏ Р°РІС‚РѕР±СѓСЃР°, stops - РІРµРєС‚РѕСЂ РµРіРѕ РѕСЃС‚Р°РЅРѕРІРѕРє
//	std::deque<Bus> buses_;
//
//	// РҐСЂР°РЅРёС‚ <РЅР°Р·РІР°РЅРёРµ_РѕСЃС‚Р°РЅРѕРІРєРё <РЅР°Р·РІР°РЅРёРµ_РѕСЃС‚Р°РЅРѕРІРєРё, РєРѕРѕСЂРґРёРЅР°С‚С‹_РѕСЃС‚Р°РЅРѕРІРєРё>>
//	std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
//
//	// РҐСЂР°РЅРёС‚ РЅР°Р·РІР°РЅРёРµ РѕСЃС‚Р°РЅРѕРІРєРё Рё Р°РІС‚РѕР±СѓСЃС‹, РєРѕС‚РѕСЂС‹Рµ РїРѕСЃРµС‰Р°СЋС‚ СЌС‚Сѓ РѕСЃС‚Р°РЅРѕРІРєСѓ
//	std::unordered_map<std::string_view, std::set<Bus*>> stop_buses_;
//
//	// СЂР°СЃСЃС‚РѕСЏРЅРёРµ РјРµР¶РґСѓ РѕСЃС‚Р°РЅРѕРІРєР°РјРё { РѕС‚, РґРѕ }
//	std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopHasher> distances_;
//};

#include <optional>

#include "../../database_manager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>  

class TransportCatalogue {
public:
    // РљРѕРЅСЃС‚СЂСѓРєС‚РѕСЂ СЃ РїРµСЂРµРґР°С‡РµР№ СЃСЃС‹Р»РєРё РЅР° РјРµРЅРµРґР¶РµСЂ Р±Р°Р·С‹ РґР°РЅРЅС‹С…
    TransportCatalogue(DatabaseManager& db_manager)
        : db_manager_(db_manager) {}

    // РџСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµ С‚РёРїР° Р°РІС‚РѕР±СѓСЃР° РІ СЃС‚СЂРѕРєСѓ Рё РѕР±СЂР°С‚РЅРѕ
    std::string BusTypeToString(BusType bus_type);
    BusType StringToBusType(const std::string& bus_type) const;

    // РњРµС‚РѕРґС‹ РґР»СЏ РґРѕР±Р°РІР»РµРЅРёСЏ Р°РІС‚РѕР±СѓСЃРѕРІ, РѕСЃС‚Р°РЅРѕРІРѕРє Рё СЂР°СЃСЃС‚РѕСЏРЅРёР№ РІ Р±Р°Р·Сѓ РґР°РЅРЅС‹С…
    void AddBus(const std::string_view name, std::vector<const Stop*>& stops, const bool is_roundtrip,
        const size_t color_index, const std::array<uint8_t, 3> rgb, const BusType bus_type,
        const uint8_t capacity, const bool has_wifi, const bool has_sockets,
        const bool is_night, const bool is_available, const uint8_t price);

    void AddStop(const std::string_view name, const detail::Coordinates& coordinates);

    void SetDistance(const Stop* from, const Stop* to, int distance);

    // РњРµС‚РѕРґС‹ РґР»СЏ РїРѕРёСЃРєР° Р°РІС‚РѕР±СѓСЃРѕРІ Рё РѕСЃС‚Р°РЅРѕРІРѕРє
    std::optional<Bus> FindBus(const std::string_view name) const;
    std::optional<Stop> FindStop(const std::string_view name) const;

    // РџРѕР»СѓС‡РµРЅРёРµ РёРЅС„РѕСЂРјР°С†РёРё Рѕ РјР°СЂС€СЂСѓС‚Рµ Р°РІС‚РѕР±СѓСЃР°
    BusInfo GetBusInfo(const std::string_view bus_name) const;

    // РџРѕР»СѓС‡РµРЅРёРµ СЂР°СЃСЃС‚РѕСЏРЅРёСЏ РјРµР¶РґСѓ РґРІСѓРјСЏ РѕСЃС‚Р°РЅРѕРІРєР°РјРё
    int GetDistance(const Stop* from, const Stop* to) const;

    std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopHasher> GetDistances();

    // РџРѕР»СѓС‡РµРЅРёРµ РѕС‚СЃРѕСЂС‚РёСЂРѕРІР°РЅРЅС‹С… РґР°РЅРЅС‹С…
    std::map<std::string/*_view*/, Bus> GetSortedBuses() const;
    std::map<std::string, Bus> GetSortedBuses(const std::string_view bus_name) const;
    std::map<std::string, Stop> GetSortedStops() const;

    size_t ComputeUniqueStopsCount(const std::string_view bus_number) const;

    std::set<const Bus*> GetBusesForStop(const std::string_view stop_name) const;

private:
    DatabaseManager& db_manager_; 
};
