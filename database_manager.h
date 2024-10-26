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

class DatabaseManager {
public:
    DatabaseManager() = default;
    
    // Конструктор, принимающий перемещающую ссылку (forwarding reference)
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

    bool Open() {
        return db_.open();
    }

    void Close() {
        db_.close();
    }

    void UpdateConnection(const QString& host, int port, const QString& db_name, const QString& username, const QString& password) {
        db_ = QSqlDatabase::addDatabase("QPSQL");
        db_.setHostName(host);
        db_.setPort(port);
        db_.setDatabaseName(db_name);
        db_.setUserName(username);
        db_.setPassword(password); 
    }

    bool StopIsExists(const QString& name) {
        QSqlQuery query;
        query.prepare("SELECT COUNT(*) FROM stops WHERE name = :name");
        query.bindValue(":name", name);
        if (query.exec() && query.next()) {
            return query.value(0).toInt() > 0;
        }
        return false;
    }

    bool AddStop(const QString& name, double latitude, double longitude)
    {
        if (StopIsExists(name)) {
            qDebug() << "Остановка уже существует:" << name;
            return false;
        }

        QSqlQuery query;
        query.prepare("INSERT INTO stops (name, latitude, longitude) VALUES (:name, :latitude, :longitude)");
        query.bindValue(":name", name);
        query.bindValue(":latitude", latitude);
        query.bindValue(":longitude", longitude);

        return query.exec();
    }

    // Проверка существования дистанции
    bool DistanceIsExists(const QString& from, const QString& to) {
        QSqlQuery query;
        query.prepare("SELECT COUNT(*) FROM distances WHERE from_stop = :from AND to_stop = :to");
        query.bindValue(":from", from);
        query.bindValue(":to", to);
        if (query.exec() && query.next()) {
            return query.value(0).toInt() > 0;
        }
        return false;
    }

    bool AddDistance(const QString& from, const QString& to, int distance)
    {
        if (DistanceIsExists(from, to)) {
            //qDebug() << "Дистанция уже существует:" << from << "->" << to;
            return false;
        }

        QSqlQuery query;
        query.prepare("INSERT INTO distances (from_stop, to_stop, distance) VALUES (:from, :to, :distance)");
        query.bindValue(":from", from);
        query.bindValue(":to", to);
        query.bindValue(":distance", distance);

        return query.exec();
    }

    // Проверка существования автобуса
    bool BusExists(const QString& name) {
        QSqlQuery query;
        query.prepare("SELECT COUNT(*) FROM buses WHERE name = :name");
        query.bindValue(":name", name);
        if (query.exec() && query.next()) {
            return query.value(0).toInt() > 0;
        }
        return false;
    }

    bool AddBus(const QString& name, const QStringList& stops, bool is_roundtrip, size_t color_index, const QString& bus_type, int capacity, bool has_wifi, bool has_sockets, bool is_night, bool is_available, int price)
    {
        if (BusExists(name)) { 
            return false;
        }

        QSqlQuery query;
        query.prepare("INSERT INTO buses (name, is_roundtrip, color_index, bus_type, capacity, has_wifi, has_sockets, is_night, is_available, price) "
            "VALUES (:name, :is_roundtrip, :color_index, :bus_type, :capacity, :has_wifi, :has_sockets, :is_night, :is_available, :price)");
        query.bindValue(":name", name);
        query.bindValue(":is_roundtrip", is_roundtrip);
        query.bindValue(":color_index", color_index);
        query.bindValue(":bus_type", bus_type);
        query.bindValue(":capacity", capacity);
        query.bindValue(":has_wifi", has_wifi);
        query.bindValue(":has_sockets", has_sockets);
        query.bindValue(":is_night", is_night);
        query.bindValue(":is_available", is_available);
        query.bindValue(":price", price);

        if (!query.exec()) {
            return false;
        }

        // Добавляем остановки автобуса в таблицу bus_stops
        int bus_id = query.lastInsertId().toInt();
        for (const QString& stop : stops) {
            if (!AddBusStop(bus_id, stop)) {
                return false;
            }
        }

        return true;
    }

    // Проверка существования связки bus_stop
    bool BusStopIsExists(int bus_id, const QString& stop_name) {
        QSqlQuery query;
        query.prepare("SELECT COUNT(*) FROM bus_stops WHERE bus_id = :bus_id AND stop_name = :stop_name");
        query.bindValue(":bus_id", bus_id);
        query.bindValue(":stop_name", stop_name);
        if (query.exec() && query.next()) {
            return query.value(0).toInt() > 0;
        }
        return false;
    }

    bool AddBusStop(int bus_id, const QString& stop_name)
    {
        if (BusStopIsExists(bus_id, stop_name)) {
            qDebug() << "Связка bus_stop уже существует:" << bus_id << "->" << stop_name;
            return false;
        }

        QSqlQuery query;
        query.prepare("INSERT INTO bus_stops (bus_id, stop_name) VALUES (:bus_id, :stop_name)");
        query.bindValue(":bus_id", bus_id);
        query.bindValue(":stop_name", stop_name);
        return query.exec();
    }

    bool UpdateRoutingSettings(const double bus_velocity, const int bus_wait_time)
    {
        constexpr double epsilon = 1e-9;
        if (bus_velocity > epsilon) {
            if (bus_wait_time > epsilon) {
                QSqlQuery query;
                query.prepare("UPDATE public.routing_settings SET bus_velocity = :bus_velocity, bus_wait_time = :bus_wait_time");
                query.bindValue(":bus_velocity", bus_velocity);
                query.bindValue(":bus_wait_time", bus_wait_time);
                return query.exec();

            }
        }
        return false;
    }

    // Выполнение запроса без возврата данных
    bool ExecuteQuery(const QString& queryStr) {
        QSqlQuery query;
        if (!query.exec(queryStr)) {
            qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
            return false;
        }
        return true;
    }

    // Выполнение запроса с возвратом данных
    QSqlQuery ExecuteSelectQuery(const QString& queryStr) {
        QSqlQuery query;
        if (!query.exec(queryStr)) {
            qDebug() << "Ошибка выполнения SELECT-запроса:" << query.lastError().text();
        }
        return query;
    }  

private:
    QSqlDatabase db_;
};

#endif // DATABASEMANAGER_H
