#pragma once

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QVariant>
#include <QDebug>
#include <utility>
#include <QMessageBox>

//#include "domain.h"
#include "C:\Users\burak\OneDrive\Documents\qt-transport-catalogue\backend\include\domain.h"

struct StopHasher {
    size_t operator()(const std::pair<const Stop*, const Stop*>& points) const {
        size_t hash_first = std::hash<const void*>{}(points.first);
        size_t hash_second = std::hash<const void*>{}(points.second);
        return hash_first + hash_second * 37;
    }
};

class DatabaseManager {
public:
    DatabaseManager();
    
    template <typename Container, typename Port>
    DatabaseManager(Container&& host, Port&& port, Container&& db_name, Container&& username, Container&& password)
    {
        db_ = QSqlDatabase::addDatabase("QPSQL");
        db_.setHostName(std::forward<Container>(host));
        db_.setPort(std::forward<Port>(port));
        db_.setDatabaseName(std::forward<Container>(db_name));
        db_.setUserName(std::forward<Container>(username));
        db_.setPassword(std::forward<Container>(password));
    }  

    bool Open();

    void Close();

    void UpdateConnection(const QString& host, int port, const QString& db_name, const QString& username, const QString& password);

    bool StopIsExists(const QString& name);

    bool AddStop(const QString& name, double latitude, double longitude);

    std::optional<Stop> FindStop(const QStringView name); 
     
    bool DistanceIsExists(const QString& from, const QString& to);

    int GetDistance(const Stop* from, const Stop* to); 

    std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopHasher> GetDistances();

    bool AddDistance(const QString& from, const QString& to, int distance);
     
    bool BusExists(const QString& name);

    bool AddBus(const QString& name, const std::vector<const Stop*>& stops, bool is_roundtrip, size_t color_index, const QString& bus_type, int capacity, bool has_wifi, bool has_sockets, bool is_night, bool is_available, int price);

    bool UpdateBus(const QString& name, const std::vector<const Stop*>& stops, bool is_roundtrip, size_t color_index, const QString& bus_type, int capacity, bool has_wifi, bool has_sockets, bool is_night, bool is_available, int price);

    std::optional<Bus> FindBus(const QStringView name); 

    // ѕроверка существовани€ св€зки bus_stop
    bool BusStopIsExists(int bus_id, const QString& stop_name);

    bool AddBusStop(int bus_id, const QString& stop_name);
    bool AddBusStop(int bus_id, int stop_id, int stop_position, const QString& stop_name);

    bool UpdateRoutingSettings(const double bus_velocity, const int bus_wait_time);

    // ¬ыполнение запроса без возврата данных
    bool ExecuteQuery(const QString& queryStr);

    // ¬ыполнение запроса с возвратом данных
    QSqlQuery ExecuteSelectQuery(const QString& queryStr);

    const int GetRowsCount(QStringView table_name) const;

private:
    QSqlDatabase db_;

    
};

#endif // DATABASEMANAGER_H
