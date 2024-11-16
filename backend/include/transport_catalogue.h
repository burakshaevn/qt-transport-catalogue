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
    void UpdateStop(const QStringView old_name, const QStringView new_name, const double latitude, const double longitude);

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

    double ComputeTfIdfForBus(const Bus* bus,
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
                               );

    template <typename Documents>
    std::vector<Stop*> ComputeTfIdfForStop(const Documents& documents_, const QStringView term) {
        std::map<Stop*, double> tf_idfs;
        int term_occurrences = 0;
        QString termLower = term.toString().toLower();

        std::vector<Stop*> exact_match_stops;

        for (const auto& [key, document] : documents_) {
            QString nameLower = document->name.toLower();
            if (nameLower.contains(termLower)) {
                Stop* stop = const_cast<Stop*>(FindStop(key));
                if (stop != nullptr) {
                    exact_match_stops.push_back(stop);
                    // Skip TF-IDF calculation for exact match
                    continue;
                }
            }

            // Split into words for TF-IDF calculation
            auto words = SplitIntoWords(nameLower);

            int count_term_in_document = std::count(words.begin(), words.end(), termLower);
            Stop* stop = const_cast<Stop*>(FindStop(key));
            if (stop != nullptr) {
                tf_idfs[stop] = static_cast<double>(count_term_in_document) / words.size();

                if (count_term_in_document > 0) {
                    ++term_occurrences;
                }
            }
        }

        if (term_occurrences == 0 && exact_match_stops.empty()) {
            return {};
        }

        double idf = 0.0;
        if (term_occurrences > 0) {
            idf = std::log(static_cast<double>(documents_.size()) / term_occurrences);
        }

        for (auto& [stop, tf] : tf_idfs) {
            tf *= idf;
        }

        std::vector<Stop*> stops(exact_match_stops);
        for (const auto& [stop, tf] : tf_idfs) {
            if (tf != 0) {
                stops.push_back(stop);
            }
        }

        std::sort(stops.begin(), stops.end(), [&tf_idfs](Stop* lhs, Stop* rhs) {
            double lhsScore = tf_idfs.count(lhs) ? tf_idfs.at(lhs) : std::numeric_limits<double>::max();
            double rhsScore = tf_idfs.count(rhs) ? tf_idfs.at(rhs) : std::numeric_limits<double>::max();
            return lhsScore > rhsScore;
        });
        return stops;
    }
	 
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
    std::unordered_map<QString, std::shared_ptr<const Stop>> stopname_to_stop_;

	// дек автобусов
	std::deque<Bus> buses_;

	// хранит <название_автобуса, указатель на автобус>
    std::unordered_map<QString, const Bus*> busname_to_bus_;

	// хранит название остановки и автобусы, которые проходят эту остановку
    std::unordered_map<QString, std::set<Bus*>> stop_buses_;

	// расстояние между остановками { от, до }
    std::unordered_map<std::pair<const QString, const QString>, int, StopHasher> distances_;
}; 
