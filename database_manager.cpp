#include "database_manager.h"

DatabaseManager::DatabaseManager() = default;

bool DatabaseManager::Open() {
    return db_.open();
}

void DatabaseManager::Close() {
    db_.close();
}

void DatabaseManager::UpdateConnection(const QString& host, int port, const QString& db_name, const QString& username, const QString& password) {
    db_ = QSqlDatabase::addDatabase("QPSQL");
    db_.setHostName(host);
    db_.setPort(port);
    db_.setDatabaseName(db_name);
    db_.setUserName(username);
    db_.setPassword(password);
}

bool DatabaseManager::StopIsExists(const QString& name) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM stops WHERE name = :name");
    query.bindValue(":name", name);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool DatabaseManager::AddStop(const QString& name, double latitude, double longitude)
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

std::optional<Stop> DatabaseManager::FindStop(const std::string_view name)
{
    QSqlQuery query = ExecuteSelectQuery(
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

// Проверка существования дистанции
bool DatabaseManager::DistanceIsExists(const QString& from, const QString& to) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM distances WHERE from_stop = :from AND to_stop = :to");
    query.bindValue(":from", from);
    query.bindValue(":to", to);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

int DatabaseManager::GetDistance(const Stop* from, const Stop* to)
{
    QSqlQuery query = ExecuteSelectQuery(
        QString("SELECT distance FROM distances WHERE from_stop = '%1' AND to_stop = '%2'")
        .arg(QString::fromStdString(from->name))
        .arg(QString::fromStdString(to->name))
    );

    if (query.next()) {
        return query.value("distance").toInt();
    }

    return 0;
}

std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopHasher> DatabaseManager::GetDistances()
{
    std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopHasher> distances;

    QSqlQuery query = ExecuteSelectQuery("SELECT from_stop, to_stop, distance FROM distances;");
    while (query.next()) {
        std::string from_stop_name = query.value("from_stop").toString().toStdString();
        std::string to_stop_name = query.value("to_stop").toString().toStdString();
        int distance = query.value("distance").toInt(); 
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

bool DatabaseManager::AddDistance(const QString& from, const QString& to, int distance)
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
bool DatabaseManager::BusExists(const QString& name) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM buses WHERE name = :name");
    query.bindValue(":name", name);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool DatabaseManager::AddBus(const QString& name, const std::vector<const Stop*>&stops, bool is_roundtrip, size_t color_index, const QString& bus_type, int capacity, bool has_wifi, bool has_sockets, bool is_night, bool is_available, int price)
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
    for (auto stop : stops) {
        if (!AddBusStop(bus_id, QString::fromStdString(stop->name))) {
            return false;
        }
    }

    return true;
}

//bool DatabaseManager::UpdateBus(const QString& name, const std::vector<const Stop*>& stops, bool is_roundtrip, size_t color_index, const QString& bus_type, int capacity, bool has_wifi, bool has_sockets, bool is_night, bool is_available, int price) {
//    QSqlDatabase::database().transaction();   
//
//    // Удаляем существующие записи для остановок этого автобуса в `bus_stops`
//    QSqlQuery deleteQuery;
//    deleteQuery.prepare("DELETE FROM bus_stops WHERE bus_id = (SELECT id FROM buses WHERE name = ':name')");
//    deleteQuery.bindValue(":name", name);
//    if (!deleteQuery.exec()) {
//        //QMessageBox::critical(this, "Ошибка", deleteQuery.lastError().text());
//        QSqlDatabase::database().rollback();  // Откатываем транзакцию в случае ошибки
//        return false;
//    }
//
//    // Если автобус уже существует, обновляем его
//    if (BusExists(name)) {
//        QSqlQuery updateQuery;
//        updateQuery.prepare("UPDATE public.buses SET name = ':name', is_roundtrip = :is_roundtrip, color_index = :color_index, bus_type = ':bus_type', capacity = :capacity, has_wifi = :has_wifi, has_sockets = :has_sockets, is_night = :is_night, is_available = :is_available, price = :price WHERE public.buses.name = ':name'; ");
//        updateQuery.bindValue(":name", name);
//        updateQuery.bindValue(":is_roundtrip", is_roundtrip);
//        updateQuery.bindValue(":color_index", color_index);
//        updateQuery.bindValue(":bus_type", bus_type);
//        updateQuery.bindValue(":capacity", capacity);
//        updateQuery.bindValue(":has_wifi", has_wifi);
//        updateQuery.bindValue(":has_sockets", has_sockets);
//        updateQuery.bindValue(":is_night", is_night);
//        updateQuery.bindValue(":is_available", is_available);
//        updateQuery.bindValue(":price", price);
//        if (!updateQuery.exec()) {
//            //qDebug() << "Ошибка обновления автобуса в базе данных:" << updateQuery.lastError().text();
//            QSqlDatabase::database().rollback();  // Откатываем транзакцию в случае ошибки
//            return false;
//        }
//    }
//    else {
//        // Если автобус не существует, добавляем его
//        if (!AddBus(name, stops, is_roundtrip, color_index, bus_type, capacity, has_wifi, has_sockets, is_night, is_available, price)) {
//            QSqlDatabase::database().rollback();  // Откатываем транзакцию в случае ошибки
//            return false;
//        }
//    }
//
//    // Получаем ID автобуса
//    QSqlQuery busIdQuery = ExecuteSelectQuery(QString("SELECT id FROM buses WHERE name = '%1'").arg(name));
//    if (!busIdQuery.next()) {
//        //qDebug() << "Ошибка поиска ID автобуса";
//        QSqlDatabase::database().rollback();  // Откатываем транзакцию в случае ошибки
//        return false;
//    }
//    int bus_id = busIdQuery.value("id").toInt();
//
//    // Вставляем новые остановки в `bus_stops`
//    for (size_t i = 0; i < stops.size(); ++i) {
//        // Получаем ID остановки
//        QSqlQuery stopIdQuery = ExecuteSelectQuery(QString("SELECT id FROM stops WHERE name = '%1'").arg(QString::fromStdString(stops[i]->name)));
//        if (!stopIdQuery.next()) {
//            //qDebug() << "Ошибка поиска ID автобуса";
//            QSqlDatabase::database().rollback();  // Откатываем транзакцию в случае ошибки
//            return false;
//        }
//        int stop_id = stopIdQuery.value("id").toInt();
//
//        if (!AddBusStop(bus_id, stop_id, i + 1, QString::fromStdString(stops[i]->name))) {
//            //qDebug() << "Ошибка добавления остановки в `bus_stops` для автобуса";
//            QSqlDatabase::database().rollback();  // Откатываем транзакцию в случае ошибки
//            return false;
//        }
//    }
//
//    QSqlDatabase::database().commit();  // Фиксируем транзакцию
//    return true;
//}
bool DatabaseManager::UpdateBus(const QString& name, const std::vector<const Stop*>& stops, bool is_roundtrip, size_t color_index, const QString& bus_type, int capacity, bool has_wifi, bool has_sockets, bool is_night, bool is_available, int price) {
    // Начало транзакции
    if (!QSqlDatabase::database().transaction()) {
        qDebug() << "Не удалось начать транзакцию:" << QSqlDatabase::database().lastError().text();
        return false;
    }

    // Удаление существующих записей
    QSqlQuery deleteQuery;
    deleteQuery.prepare("DELETE FROM bus_stops WHERE bus_id = (SELECT id FROM buses WHERE name = :name)");
    deleteQuery.bindValue(":name", name);
    if (!deleteQuery.exec()) {
        qDebug() << "Ошибка удаления остановок:" << deleteQuery.lastError().text();
        QSqlDatabase::database().rollback();
        return false;
    }

    // Обновление данных автобуса
    if (BusExists(name)) {
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE buses SET is_roundtrip = :is_roundtrip, color_index = :color_index, bus_type = :bus_type, capacity = :capacity, has_wifi = :has_wifi, has_sockets = :has_sockets, is_night = :is_night, is_available = :is_available, price = :price WHERE name = :name");
        updateQuery.bindValue(":name", name);
        updateQuery.bindValue(":is_roundtrip", is_roundtrip);
        updateQuery.bindValue(":color_index", color_index);
        updateQuery.bindValue(":bus_type", bus_type);
        updateQuery.bindValue(":capacity", capacity);
        updateQuery.bindValue(":has_wifi", has_wifi);
        updateQuery.bindValue(":has_sockets", has_sockets);
        updateQuery.bindValue(":is_night", is_night);
        updateQuery.bindValue(":is_available", is_available);
        updateQuery.bindValue(":price", price);
        if (!updateQuery.exec()) {
            //qDebug() << "Ошибка обновления автобуса:" << updateQuery.lastError().text();
            QSqlDatabase::database().rollback();
            return false;
        }
    }
    else {
        // Логика добавления нового автобуса, если его не существует
        if (!AddBus(name, stops, is_roundtrip, color_index, bus_type, capacity, has_wifi, has_sockets, is_night, is_available, price)) {
            QSqlDatabase::database().rollback();
            return false;
        }
    }

    // Получение ID автобуса
    QSqlQuery busIdQuery;
    busIdQuery.prepare("SELECT id FROM buses WHERE name = :name");
    busIdQuery.bindValue(":name", name);
    if (!busIdQuery.exec() || !busIdQuery.next()) {
        //qDebug() << "Ошибка получения ID автобуса:" << busIdQuery.lastError().text();
        QSqlDatabase::database().rollback();
        return false;
    }
    int bus_id = busIdQuery.value(0).toInt();

    // Вставка новых остановок
    for (size_t i = 0; i < stops.size(); ++i) {
        QSqlQuery stopIdQuery;
        stopIdQuery.prepare("SELECT id FROM stops WHERE name = :name");
        stopIdQuery.bindValue(":name", QString::fromStdString(stops[i]->name));
        if (!stopIdQuery.exec() || !stopIdQuery.next()) {
            //qDebug() << "Ошибка получения ID остановки:" << stopIdQuery.lastError().text();
            QSqlDatabase::database().rollback();
            return false;
        }
        int stop_id = stopIdQuery.value(0).toInt();

        if (!AddBusStop(bus_id, stop_id, i + 1, QString::fromStdString(stops[i]->name))) {
            //qDebug() << "Ошибка добавления остановки:" << QSqlDatabase::database().lastError().text();
            QSqlDatabase::database().rollback();
            return false;
        }
    }

    // Завершение транзакции
    if (!QSqlDatabase::database().commit()) {
        //qDebug() << "Ошибка фиксации транзакции:" << QSqlDatabase::database().lastError().text();
        return false;
    }

    return true;
}
std::optional<Bus> DatabaseManager::FindBus(const std::string_view name)
{
    QSqlQuery query = ExecuteSelectQuery(
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

        QSqlQuery stops_query = ExecuteSelectQuery(
            QString("SELECT s.name, s.latitude, s.longitude FROM bus_stops bs JOIN stops s ON bs.stop_id = s.id WHERE bs.bus_id = %1;").arg(query.value("id").toInt())
        );

        static std::vector<std::unique_ptr<Stop>> stops_storage;
        while (stops_query.next()) { 
            auto stop_ptr = std::make_unique<Stop>();
            stop_ptr->name = stops_query.value("name").toString().toStdString();
            stop_ptr->coords.lat = stops_query.value("latitude").toDouble();
            stop_ptr->coords.lng = stops_query.value("longitude").toDouble();
             
            bus.stops.emplace_back(stop_ptr.get());
            stops_storage.emplace_back(std::move(stop_ptr));
        }
        return bus;
    }
    return std::nullopt;
}

// Проверка существования связки bus_stop
bool DatabaseManager::BusStopIsExists(int bus_id, const QString& stop_name) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM bus_stops WHERE bus_id = :bus_id AND stop_id = (SELECT id FROM stops WHERE name = ':stop_name'); ");
    query.bindValue(":bus_id", bus_id);
    query.bindValue(":stop_name", stop_name);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool DatabaseManager::AddBusStop(int bus_id, const QString& stop_name) {
    if (BusStopIsExists(bus_id, stop_name)) {
        qDebug() << "Связка bus_stop уже существует:" << bus_id << "->" << stop_name;
        return true;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO bus_stops (bus_id, stop_id) VALUES (:bus_id, :stop_id)");
    query.bindValue(":bus_id", bus_id);
    query.bindValue(":stop_id", stop_name);
    return query.exec();
}
bool DatabaseManager::AddBusStop(int bus_id, int stop_id, int stop_position, const QString& stop_name)
{
    if (BusStopIsExists(bus_id, stop_name)) { 
        return true;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO bus_stops VALUES (:bus_id, :stop_id, :stop_position)");
    query.bindValue(":bus_id", bus_id);
    query.bindValue(":stop_id", stop_id);
    query.bindValue(":stop_position", stop_position);
    auto tmp = query.lastError().text();
    return query.exec();
}

bool DatabaseManager::UpdateRoutingSettings(const double bus_velocity, const int bus_wait_time)
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
bool DatabaseManager::ExecuteQuery(const QString& queryStr) {
    QSqlQuery query;
    if (!query.exec(queryStr)) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        return false;
    }
    return true;
}

// Выполнение запроса с возвратом данных
QSqlQuery DatabaseManager::ExecuteSelectQuery(const QString& queryStr) {
    QSqlQuery query;
    if (!query.exec(queryStr)) {
        qDebug() << "Ошибка выполнения SELECT-запроса:" << query.lastError().text();
    }
    return query;
}

const int DatabaseManager::GetRowsCount(std::string_view table_name) const
{
   QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM information_schema.columns WHERE table_name = :table_name;");
    query.bindValue(":table_name", QString::fromStdString(std::string(table_name)));
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return false;
}
 
