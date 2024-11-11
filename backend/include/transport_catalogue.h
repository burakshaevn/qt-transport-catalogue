#pragma once

#include "database_manager.h"
#include "domain.h"
#include <deque>
#include <iostream>
#include <set> 
#include <map> 
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "geo.h"
#include <optional>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QObject>

// РєР»Р°СЃСЃ С‚СЂР°РЅСЃРїРѕСЂС‚РЅРѕРіРѕ СЃРїСЂР°РІРѕС‡РЅРёРєР° 

struct StopHasher { 
	size_t operator()(const std::pair<const QString, const QString>& stops) const {
		return qHash(stops.first) ^ (qHash(stops.second) << 1);
	} 
};

class TransportCatalogue { 
public: 

	TransportCatalogue(DatabaseManager& db_manager)
		: db_manager_(db_manager) 
	{}

    void AddBus(Bus& bus);
    void DeleteBus(Bus* bus);

	void AddStop(const QStringView name, const detail::Coordinates& coordinates);
    void AddStopForBus(const Stop* stop);
    void DeleteStop(Stop* stop);

	const Bus* FindBus(const QStringView name) const;
    const Stop* FindStop(const QStringView name) const;

    const BusInfo GetBusInfo(const Bus* current_bus) const;

    const std::set<Bus*> GetBusesForStop(const QStringView name) const;
    std::set<Bus*> GetBusesForStop(const QStringView name);

    const std::vector<const Stop*> GetStopsForBus(const QStringView name) const;
    std::vector<const Stop*> GetStopsForBus(const QStringView name);

    void SetDistance(const QStringView from, const QStringView to, int distance);
    int GetDistance(const QStringView  from, const QStringView to) const;

	const std::map<QStringView, const Bus*> GetSortedBuses() const; 
	[[maybe_unused]] const std::map<QStringView, const Bus*> GetSortedBuses(const QStringView bus_name) const;

	const std::map<QStringView, const Stop*> GetSortedStops() const;
	 
	void UpdateStops(); 
	void UpdateStopnameToStop(); 
	void UpdateBuses();
	void UpdateBus(const Bus* bus);
	 
	void UpdateBusnameToBus(); 
	void UpdateStopBuses(); 
	void UpdateDistances();

private:
	friend class RequestHandler;

	DatabaseManager& db_manager_; 

	size_t ComputeUniqueStopsCount(QStringView bus_number) const;
	
	// дек остановок
	std::deque<Stop> stops_;

	// хранит <название_остановки, указатель на остановку>
    std::unordered_map<QString, const Stop*> stopname_to_stop_;

	// дек автобусов
	std::deque<Bus> buses_;

	// хранит <название_автобуса, указатель на автобус>
    std::unordered_map<QString, const Bus*> busname_to_bus_;

	// хранит название остановки и автобусы, которые проходят эту остановку
    std::unordered_map<QString, std::set<Bus*>> stop_buses_;

	// расстояние между остановками { от, до }
    std::unordered_map<std::pair<const QString, const QString>, int, StopHasher> distances_;
}; 
