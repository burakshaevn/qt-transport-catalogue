#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "database_manager.h" 

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , db_manager_()
    , transport_catalogue_(db_manager_)
{
    ui->setupUi(this);
    this->setFixedSize(1300, 760);


    // Устанавливаем начальное окно при запуске — База данных
    ui->stackedWidget->setCurrentWidget(ui->database);

    SetLineEditSettings();
    SetLabelSettings();
}

MainWindow::~MainWindow() = default;  

DatabaseManager MainWindow::GetDatabaseManager()
{
    return db_manager_;
}

void MainWindow::DisplayMapOnLabel(const std::string& bus_name, QLabel* label) {
    auto feedback = JsonToSVG(bus_name);

    if (std::holds_alternative<QString>(feedback)) {
        QGraphicsScene* scene = new QGraphicsScene(this);
        QGraphicsView* view = new QGraphicsView(scene, label);

        QGraphicsSvgItem* svgItem = new QGraphicsSvgItem(std::get<QString>(feedback));
        scene->addItem(svgItem);

        view->setSceneRect(svgItem->boundingRect());
        view->setRenderHint(QPainter::Antialiasing);
        view->setDragMode(QGraphicsView::ScrollHandDrag);
        view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

        // Устанавливаем размер для QGraphicsView
        view->setFixedSize(1280, 716); // Установите нужный размер

        // Устанавливаем стиль
        view->setStyleSheet("border: 0px solid;"); 

        // Устанавливаем масштабирование, если это необходимо
        qreal scaleFactor = 0.7;// 1.5;
        view->scale(scaleFactor, scaleFactor);
        view->move(10, 8);
        // Отображаем QGraphicsView внутри QLabel
        label->setLayout(new QVBoxLayout);
        label->layout()->addWidget(view);
    }
    else if (std::holds_alternative<std::runtime_error>(feedback)) {
        qDebug() << "Не удалось открыть файл для записи.";
    }
}

// Функция для парсинга строки и преобразования её в QVariantList
QVariantList ParseArrayString(const QString& array_str) {
    QVariantList result;

    // Убираем фигурные скобки
    QString clean_str = array_str.mid(1, array_str.size() - 2); // Убираем первые и последние скобки

    // Используем регулярное выражение для разделения элементов
    QStringList elements = clean_str.split(QRegularExpression("\\s*,\\s*"));

    // Преобразуем элементы в числа и добавляем в результат
    for (const QString& element : elements) {
        result.push_back(element.toDouble()); // Преобразуем в double, можно использовать toInt() для целых чисел
    }

    return result;
}

MainWindow::Value MainWindow::JsonToSVG(const std::string& bus_name) {
    QString appDir = QCoreApplication::applicationDirPath();

    json::Dict render_settings;

    QSqlQuery query_render = db_manager_.ExecuteSelectQuery(
        QString("SELECT * FROM render_settings;")
    );

    if (query_render.next()) {
        // Извлечение обычных значений
        render_settings["bus_label_font_size"] = json::Node(query_render.value("bus_label_font_size").toInt());

        // Извлечение массива bus_label_offset через строку и парсинг
        QString bus_label_offset_str = query_render.value("bus_label_offset").toString();
        QVariantList bus_label_offset_list = ParseArrayString(bus_label_offset_str);
        json::Array bus_label_offset;
        bus_label_offset.push_back(json::Node(bus_label_offset_list[0].toDouble()));
        bus_label_offset.push_back(json::Node(bus_label_offset_list[1].toDouble()));
        render_settings["bus_label_offset"] = bus_label_offset;

        // Извлечение stop_label_font_size
        render_settings["stop_label_font_size"] = json::Node(query_render.value("stop_label_font_size").toInt());

        // Извлечение массива stop_label_offset через строку и парсинг
        QString stop_label_offset_str = query_render.value("stop_label_offset").toString();
        QVariantList stop_label_offset_list = ParseArrayString(stop_label_offset_str);
        json::Array stop_label_offset;
        stop_label_offset.push_back(json::Node(stop_label_offset_list[0].toDouble()));
        stop_label_offset.push_back(json::Node(stop_label_offset_list[1].toDouble()));
        render_settings["stop_label_offset"] = stop_label_offset;

        // Извлечение stop_radius
        render_settings["stop_radius"] = json::Node(query_render.value("stop_radius").toInt());

        // Извлечение массива underlayer_color через строку и парсинг
        QString underlayer_color_str = query_render.value("underlayer_color").toString();
        QVariantList underlayer_color_list = ParseArrayString(underlayer_color_str);
        json::Array underlayer_color;
        underlayer_color.push_back(json::Node(underlayer_color_list[0].toInt()));
        underlayer_color.push_back(json::Node(underlayer_color_list[1].toInt()));
        underlayer_color.push_back(json::Node(underlayer_color_list[2].toInt()));
        underlayer_color.push_back(json::Node(underlayer_color_list[3].toDouble()));
        render_settings["underlayer_color"] = underlayer_color;

        // Извлечение остальных параметров
        render_settings["underlayer_width"] = json::Node(query_render.value("underlayer_width").toDouble());
        render_settings["line_width"] = json::Node(query_render.value("line_width").toDouble());
        render_settings["padding"] = json::Node(query_render.value("padding").toInt());
        render_settings["height"] = json::Node(query_render.value("height").toInt());
        render_settings["width"] = json::Node(query_render.value("width").toInt());

        // Извлечение цветовой палитры из базы данных
        QSqlQuery query_palette = db_manager_.ExecuteSelectQuery(
            QString("SELECT color FROM color_palette WHERE render_setting_id = %1;")
            .arg(query_render.value("id").toInt())
        );

        json::Array color_palette;
        while (query_palette.next()) {
            QString color_str = query_palette.value("color").toString();
            QVariantList color_list = ParseArrayString(color_str);
            json::Array color;
            color.push_back(json::Node(color_list[0].toInt()));
            color.push_back(json::Node(color_list[1].toInt()));
            color.push_back(json::Node(color_list[2].toInt()));
            color_palette.push_back(color);
        }
        render_settings["color_palette"] = color_palette;
    }

    QSqlQuery query_routing_settings = db_manager_.ExecuteSelectQuery(
        QString("SELECT * FROM routing_settings;")
    );

    if (query_routing_settings.next()) {
        const auto& router = TransportRouter{
            std::move(query_routing_settings.value("bus_wait_time").toInt()),
            std::move(query_routing_settings.value("bus_velocity").toDouble()),
            transport_catalogue_
        };

        std::istringstream fake_input(R"({})");
        JsonReader json_reader(fake_input);

        renderer::MapRenderer map_renderer(json_reader.PullRenderSettings(render_settings));
            
        RequestHandler rh(transport_catalogue_, map_renderer, router);

        // Генерация и сохранение SVG
        QString file_path = appDir + "/../../map-renderer/map.svg";
        QFile svg_file(file_path);

        if (!svg_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Не удалось открыть файл для записи.";
            throw std::runtime_error("Не удалось открыть файл для записи.");
        }

        QTextStream out(&svg_file);
        std::stringstream svg_data;
        rh.RenderMap(bus_name).Render(svg_data);
        out << QString::fromStdString(svg_data.str());

        svg_file.close();

        qDebug() << "SVG успешно сохранён в " << file_path;

        return file_path;
    }
    return {};
}

void MainWindow::on_button_buses_clicked()
{
    if (db_manager_.Open()) {
        ui->stackedWidget->setCurrentWidget(ui->buses);
        QLayout* existingLayout = ui->scrollAreaWidgetContents->layout();
        if (existingLayout != nullptr) {
            QLayoutItem* item;
            while ((item = existingLayout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
            delete existingLayout;
        }
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setAlignment(Qt::AlignTop);
        for (auto& [bus_name, bus_ptr] : transport_catalogue_.GetSortedBuses()) {
            DrawBus(&bus_ptr, layout);
        }
        ui->scrollAreaWidgetContents->setLayout(layout);
    }
    else { 
        QMessageBox::critical(this, "Данные не загружены", "Выполните подключение к базе данных, чтобы отобразить данные.");
    }
}

void MainWindow::on_button_map_clicked()
{
    if (db_manager_.Open()) {
        ui->stackedWidget->setCurrentWidget(ui->map);
        DisplayMapOnLabel("", ui->label);
    }
    else {
        QMessageBox::critical(this, "Данные не загружены", "Выполните подключение к базе данных, чтобы отобразить данные.");
    }
}

void MainWindow::on_button_stops_clicked()
{
    if (db_manager_.Open()) {
        ui->stackedWidget->setCurrentWidget(ui->stops);
        QLayout* existingLayout = ui->scrollAreaWidgetContents_2->layout();
        if (existingLayout != nullptr) {
            QLayoutItem* item;
            while ((item = existingLayout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
            delete existingLayout;
        }
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setAlignment(Qt::AlignTop);
        for (auto& [stop_name, stop_ptr] : transport_catalogue_.GetSortedStops()) {
            DrawStop(stop_ptr, layout);
        }
        ui->scrollAreaWidgetContents_2->setLayout(layout);
    }
    else {
        QMessageBox::critical(this, "Данные не загружены", "Выполните подключение к базе данных, чтобы отобразить данные.");
    }
}

void MainWindow::on_button_db_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->database);
}

void MainWindow::SetLineEditSettings(){
    for (auto lineEdit : { ui->lineEdit_busname, ui->lineEdit_capacity, ui->lineEdit_price, ui->lineEdit_stopname }) {
        lineEdit->setAlignment(Qt::AlignCenter);
    }
    ui->lineEdit_busname->setMaxLength(3);
    ui->lineEdit_stopname->setMaxLength(100);  

    auto database = ui->stackedWidget->widget(0); 
    database->findChild<QLineEdit*>("lineEdit_password")->setEchoMode(QLineEdit::Password);
}

void MainWindow::SetLabelSettings(){
    auto page_database = ui->stackedWidget->widget(0);
    auto page_stops = ui->stackedWidget->widget(1);
    for (auto label : { 
        ui->label_4, 
        ui->label_6,   
        page_stops->findChild<QLabel*>("label_9"), 
        page_stops->findChild<QLabel*>("label_11"), 
        page_stops->findChild<QLabel*>("label_12"), 
        page_database->findChild<QLabel*>("label_6")
        
        }) 
    {
        label->setAlignment(Qt::AlignCenter);
    }   
   
    page_database->findChild<QLineEdit*>("lineEdit_password")->setEchoMode(QLineEdit::Password);
}

void MainWindow::on_reset_all_filters_clicked()
{
    for(auto button_ : { ui->is_available, ui->is_unavailable }){
        button_->setAutoExclusive(false); // Отключаем автоэксклюзивность
        button_->setChecked(false);       // Сбрасываем состояние
        button_->setAutoExclusive(true);  // Восстанавливаем автоэксклюзивность
    }
    for (auto button_ : {
        ui->is_undefined,
        ui->is_autobus, 
        ui->is_trolleybus, 
        ui->is_electrobus, 
        ui->is_wifi, 
        ui->is_no_wifi,
        ui->is_sockets, 
        ui->is_no_sockets, 
        ui->is_night_route, 
        ui->is_day_route, 
        ui->is_roundtrip,
        ui->is_non_roundtrip
        })
    {
        button_->setChecked(false);
    }
    ui->lineEdit_busname->setText("");
    ui->lineEdit_capacity->setText("");
    ui->lineEdit_price->setText("");
    ui->lineEdit_stopname->setText("");
    
    QLayout* existingLayout = ui->scrollAreaWidgetContents->layout();
    if (existingLayout != nullptr) {
        QLayoutItem* item;
        while ((item = existingLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete existingLayout;
    }
    QVBoxLayout* layout = new QVBoxLayout();
    for (auto& [bus_name, bus_ptr] : transport_catalogue_.GetSortedBuses()) {
        DrawBus(&bus_ptr, layout);
    }
    ui->scrollAreaWidgetContents->setLayout(layout);
}

void MainWindow::on_search_bus_clicked()
{
    std::optional<bool> is_available;
    if (ui->is_available->isChecked()) {
        is_available = true;
    }
    else if (ui->is_unavailable->isChecked()) {
        is_available = false;
    }
    else {
        is_available = std::nullopt;
    }
    
    std::optional<bool> is_roundtrip;
    if (ui->is_roundtrip->isChecked()) {
        is_roundtrip = true;
    }
    else if (ui->is_non_roundtrip->isChecked()) {
        is_roundtrip = false;
    }
    else {
        is_roundtrip = std::nullopt;
    }

    // Определяем, какой тип маршрута ищется
    std::set<BusType> selected_bus_types;
    if (ui->is_undefined->isChecked()) {
        selected_bus_types.insert(BusType::Autobus);
        selected_bus_types.insert(BusType::Electrobus);
        selected_bus_types.insert(BusType::Trolleybus);
    }
    else if (ui->is_autobus->isChecked()) {
        selected_bus_types.insert(BusType::Autobus);
    }
    else if (ui->is_electrobus->isChecked()) {
        selected_bus_types.insert(BusType::Electrobus);
    }
    else if (ui->is_trolleybus->isChecked()) {
        selected_bus_types.insert(BusType::Trolleybus);
    }

    std::optional<bool> is_wifi;
    if (ui->is_wifi->isChecked()) {
        is_wifi = true;
    }
    else if (ui->is_no_wifi->isChecked()) {
        is_wifi = false;
    }
    else {
        is_wifi = std::nullopt;
    }
    
    std::optional<bool> is_sockets;
    if (ui->is_sockets->isChecked()) {
        is_sockets = true;
    }
    else if (ui->is_no_sockets->isChecked()) {
        is_sockets = false;
    }
    else {
        is_sockets = std::nullopt;
    }
    
    std::optional<bool> is_night_routeis;
    if (ui->is_night_route->isChecked()) {
        is_night_routeis = true;
    }
    else if (ui->is_day_route->isChecked()) {
        is_night_routeis = false;
    }
    else {
        is_night_routeis = std::nullopt;
    }
    
    GetRelevantBuses(ui->lineEdit_busname->text().toStdString(),
        { ui->lineEdit_stopname->text().toStdString() },
        is_roundtrip,
        -1,
        selected_bus_types,
        static_cast<uint8_t>(ui->lineEdit_capacity->text().toInt()),
        is_wifi,
        is_sockets,
        is_night_routeis, 
        is_available,
        static_cast<uint8_t>(ui->lineEdit_price->text().toInt())
    );
}

void MainWindow::GetRelevantBuses(
    const std::string_view name,
    const std::string_view desired_stop,
    const std::optional<bool> is_roundtrip,
    const size_t color_index,
    const std::set<BusType>& bus_types,
    const uint8_t capacity,
    const std::optional<bool> has_wifi,
    const std::optional<bool> has_sockets,
    const std::optional<bool> is_night,
    const std::optional<bool> is_available,
    const uint8_t price
) {

    // Очищаем существующие виджеты в scrollArea
    QLayout* existingLayout = ui->scrollAreaWidgetContents->layout();
    if (existingLayout != nullptr) {
        QLayoutItem* item;
        while ((item = existingLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete existingLayout;
    }

    // Функция для подсчёта релевантности для каждого автобуса
    auto compute_tf_idf_for_bus = [&](const Bus& bus) -> double {
        int count_term_in_bus = 0;
        int total_terms = 0; // Количество параметров, которые учитываются

        if (!name.empty()) {
            ++total_terms;
            if (bus.name == name) {
                ++count_term_in_bus;
            }
        }
        if (!desired_stop.empty()) {
            ++total_terms;
            for (const auto& stop : bus.stops) {
                if (stop->name == desired_stop) {
                    ++count_term_in_bus;
                    break;
                }
            }
        }
        if (capacity > 0) { // Проверка, если вместимость указана
            ++total_terms;
            if (bus.capacity == capacity) {
                ++count_term_in_bus;
            }
        }
        if (price > 0) { // Проверка, если цена указана
            ++total_terms;
            if (bus.price == price) {
                ++count_term_in_bus;
            }
        }
        if (has_wifi) { // Проверяем только если пользователь выбрал Wi-Fi
            ++total_terms;
            if (bus.has_wifi == has_wifi) {
                ++count_term_in_bus;
            }
        }
        if (has_sockets) { // Проверка наличия розеток
            ++total_terms;
            if (bus.has_sockets == has_sockets) {
                ++count_term_in_bus;
            }
        }
        if (is_night) { // Проверка, ночной маршрут или нет
            ++total_terms;
            if (bus.is_night == is_night) {
                ++count_term_in_bus;
            }
        }
        if (is_roundtrip) { // Проверка, кольцевой или нет
            ++total_terms;
            if (bus.is_roundtrip == is_roundtrip) {
                ++count_term_in_bus;
            }
        }

        // Проверка доступности
        if (is_available.has_value()) {
            ++total_terms;
            if (bus.is_available == is_available) {
                ++count_term_in_bus;
            }
        }

        // Проверяем, заданы ли типы автобусов
        if (!bus_types.empty()) {
            ++total_terms;
            if (bus_types.find(bus.bus_type) != bus_types.end()) {
                ++count_term_in_bus;
            }
        }

        // Если параметры не заданы, игнорируем этот автобус
        if (total_terms == 0) {
            return 0.0;
        }

        // TF: отношение совпадений к общему числу проверяемых параметров
        return (static_cast<double>(count_term_in_bus) / total_terms) * 1.;
        };

    QVBoxLayout* layout = new QVBoxLayout(ui->scrollAreaWidgetContents);
    layout->setAlignment(Qt::AlignTop);

    auto buses = transport_catalogue_.GetSortedBuses();

    std::multimap<double, Bus*, std::greater<double>> relevant_buses;

    for (auto& [bus_name, bus] : buses) {
        double tf_idf = compute_tf_idf_for_bus(bus);
        if (tf_idf > 0.0) {
            relevant_buses.emplace(tf_idf, &bus);
        }
    }

    for (auto& [tf_idf, bus] : relevant_buses) {
        DrawBus(bus, layout);
    }

    ui->scrollAreaWidgetContents->setLayout(layout);
}

void MainWindow::DrawBus(Bus* bus, QVBoxLayout* layout) {
    // Создаем общий указатель на Bus, передавая raw-показатель
    std::shared_ptr<Bus> bus_ptr(bus);//std::shared_ptr<Bus>(bus);

    // Лейбл, на котором будут располагаться блоки с информацией о маршруте
    QLabel* backgroundLabel = new QLabel(ui->scrollAreaWidgetContents);
    backgroundLabel->setStyleSheet("background-color: #FFFFFF;");
    backgroundLabel->setFixedSize(631, 100);

    // Номер автобуса
    QLabel* numberLabel = new QLabel(QString::fromStdString(bus_ptr->name), backgroundLabel);
    numberLabel->setStyleSheet("color: #2E1C0C; font: 500 20pt 'JetBrains Mono';");
    numberLabel->setFixedSize(70, 31);
    numberLabel->setAlignment(Qt::AlignLeft);
    numberLabel->move(12, 34);

    // информация о маршруте: тип автобуса, количество мест и остановок
    QString infoText;
    switch (bus_ptr->bus_type) {
    case BusType::Autobus:
        infoText += "Автобус\n";
        break;
    case BusType::Electrobus:
        infoText += "Электробус\n";
        break;
    case BusType::Trolleybus:
        infoText += "Троллейбус\n";
        break;
    default:
        infoText += "Неопределён\n";
        break;
    }
    infoText += (QString::number(bus_ptr->capacity) + " мест\n");
    infoText += (QString::number(bus_ptr->stops.size()) + " остановок");

    QLabel* infoLabel = new QLabel(infoText, backgroundLabel);
    infoLabel->setStyleSheet("color: #2E1C0C; font: 500 10pt 'JetBrains Mono';");
    infoLabel->setFixedSize(138, 57);
    infoLabel->setAlignment(Qt::AlignLeft);
    infoLabel->move(84, 22);

    // кольцевой/некольцевой, длина и кривизна маршрута
    BusInfo bus_info = transport_catalogue_.GetBusInfo(bus_ptr->name);
    QString infoText2;
    infoText2 += (bus_ptr->is_roundtrip ? "Кольцевой\n" : "Некольцевой\n");
    infoText2 += ("Длина " + QString::number(bus_info.len) + " м\n");
    infoText2 += ("Кривизна " + QString::number(bus_info.curvature));

    QLabel* infoLabel2 = new QLabel(infoText2, backgroundLabel);
    infoLabel2->setStyleSheet("color: #2E1C0C; font: 500 10pt 'JetBrains Mono';");
    infoLabel2->setFixedSize(138, 57);
    infoLabel2->setAlignment(Qt::AlignLeft);
    infoLabel2->move(210, 22);

    // наличие wi-fi,  розеток и ночной ли маршрут
    QString infoText3;
    infoText3 += (bus_ptr->has_wifi ? "Есть Wi-Fi\n" : "Нет Wi-Fi\n");
    infoText3 += (bus_ptr->has_sockets ? "Есть розетка\n" : "Нет розетки\n");
    infoText3 += (bus_ptr->is_night ? "Ночной маршрут" : "Дневной маршрут");

    QLabel* infoLabel3 = new QLabel(infoText3, backgroundLabel);
    infoLabel3->setStyleSheet("color: #2E1C0C; font: 500 10pt 'JetBrains Mono';");
    infoLabel3->setFixedSize(153, 57);
    infoLabel3->setAlignment(Qt::AlignLeft);
    infoLabel3->move(370, 22);

    // цена
    QLabel* priceLabel = new QLabel(QString::number(bus_ptr->price) + " руб.", backgroundLabel);
    priceLabel->setStyleSheet("color: #2E1C0C; font: 500 15pt 'JetBrains Mono';");
    priceLabel->setFixedSize(125, 23);
    priceLabel->setAlignment(Qt::AlignRight);
    priceLabel->move(493, 63);

    // доступность
    QLabel* availableLabel = new QLabel(bus_ptr->is_available ? "Доступен" : "Недоступен", backgroundLabel);
    availableLabel->setStyleSheet(bus_ptr->is_available ? "color: #00BF63; font: 500 10pt 'JetBrains Mono';" : "color: #FF3131; font: 500 10pt 'JetBrains Mono';");
    availableLabel->setFixedSize(102, 17);
    availableLabel->setAlignment(Qt::AlignRight);
    availableLabel->move(514, 15);

    QPushButton* select_ = new QPushButton(backgroundLabel);
    select_->setIcon(QIcon(":/resources/edit_20x20.png"));
    select_->setIconSize(QSize(20, 20));
    select_->setFixedSize(20, 20);
    select_->move(8, 3);
    select_->setStyleSheet("border-radius: 0px;");

    // Используем shared_ptr в лямбде, чтобы избежать проблем с инвалидированием
    connect(select_, &QPushButton::clicked, [this, bus_ptr]() {
        OpenEditBusDialog(bus_ptr);
        });

    layout->addWidget(backgroundLabel);
}

//void MainWindow::DrawBus(Bus* bus, QVBoxLayout* layout) {
//    // Лейбл, на котором будут располагаться блоки с информацией о маршруте
//    QLabel* backgroundLabel = new QLabel(ui->scrollAreaWidgetContents);
//    backgroundLabel->setStyleSheet("background-color: #FFFFFF;");
//    backgroundLabel->setFixedSize(631, 100);
//
//    // Номер автобуса
//    QLabel* numberLabel = new QLabel(QString::fromStdString(bus->name), backgroundLabel);
//    numberLabel->setStyleSheet("color: #2E1C0C; font: 500 20pt 'JetBrains Mono';");
//    numberLabel->setFixedSize(70, 31);
//    numberLabel->setAlignment(Qt::AlignLeft);
//    numberLabel->move(12, 34);
//
//    // информация о маршруте: тип автобуса, количество мест и остановок
//    QString infoText;
//    switch (bus->bus_type) {
//    case BusType::Autobus:
//        infoText += "Автобус\n";
//        break;
//    case BusType::Electrobus:
//        infoText += "Электробус\n";
//        break;
//    case BusType::Trolleybus:
//        infoText += "Троллейбус\n";
//        break;
//    default:
//        infoText += "Неопределён\n";
//        break;
//    }
//    infoText += (QString::number(bus->capacity) + " мест\n");
//    infoText += (QString::number(bus->stops.size()) + " остановок");
//
//    QLabel* infoLabel = new QLabel(infoText, backgroundLabel);
//    infoLabel->setStyleSheet("color: #2E1C0C; font: 500 10pt 'JetBrains Mono';");
//    infoLabel->setFixedSize(138, 57);
//    infoLabel->setAlignment(Qt::AlignLeft);
//    infoLabel->move(84, 22);
//
//    // кольцевой/некольцевой, длина и кривизна маршрута
//    BusInfo bus_info = transport_catalogue_.GetBusInfo(bus->name);
//    QString infoText2;
//    infoText2 += (bus->is_roundtrip ? "Кольцевой\n" : "Некольцевой\n");
//    infoText2 += ("Длина " + QString::number(bus_info.len) + " м\n");
//    infoText2 += ("Кривизна " + QString::number(bus_info.curvature));
//
//    QLabel* infoLabel2 = new QLabel(infoText2, backgroundLabel);
//    infoLabel2->setStyleSheet("color: #2E1C0C; font: 500 10pt 'JetBrains Mono';");
//    infoLabel2->setFixedSize(138, 57);
//    infoLabel2->setAlignment(Qt::AlignLeft);
//    infoLabel2->move(210, 22);
//
//    // наличие wi-fi,  розеток и ночной ли маршрут
//    QString infoText3;
//    infoText3 += (bus->has_wifi ? "Есть Wi-Fi\n" : "Нет Wi-Fi\n");
//    infoText3 += (bus->has_sockets ? "Есть розетка\n" : "Нет розетки\n");
//    infoText3 += (bus->is_night ? "Ночной маршрут" : "Дневной маршрут");
//
//    QLabel* infoLabel3 = new QLabel(infoText3, backgroundLabel);
//    infoLabel3->setStyleSheet("color: #2E1C0C; font: 500 10pt 'JetBrains Mono';");
//    infoLabel3->setFixedSize(153, 57);
//    infoLabel3->setAlignment(Qt::AlignLeft);
//    infoLabel3->move(370, 22);
//
//    // цена
//    QLabel* priceLabel = new QLabel(QString::number(bus->price) + " руб.", backgroundLabel);
//    priceLabel->setStyleSheet("color: #2E1C0C; font: 500 15pt 'JetBrains Mono';");
//    priceLabel->setFixedSize(125, 23);
//    priceLabel->setAlignment(Qt::AlignRight);
//    priceLabel->move(493, 63);
//
//    // доступность
//    QLabel* availableLabel = new QLabel(bus->is_available ? "Доступен" : "Недоступен", backgroundLabel);
//    availableLabel->setStyleSheet(bus->is_available ? "color: #00BF63; font: 500 10pt 'JetBrains Mono';" : "color: #FF3131; font: 500 10pt 'JetBrains Mono';");
//    availableLabel->setFixedSize(102, 17);
//    availableLabel->setAlignment(Qt::AlignRight);
//    availableLabel->move(514, 15);
//
//    QPushButton* select_ = new QPushButton(backgroundLabel); 
//    select_->setIcon(QIcon(":/resources/edit_20x20.png"));   
//    select_->setIconSize(QSize(20, 20));                    
//    select_->setFixedSize(20, 20);                           
//    select_->move(8, 3);                                     
//    select_->setStyleSheet("border-radius: 0px;");           
//
//    std::shared_ptr<Bus> bus_ptr = std::make_shared<Bus>(bus);
//    
//    connect(select_, &QPushButton::clicked, [this, bus]() {
//        OpenEditBusDialog(bus_ptr);
//        });
//
//    layout->addWidget(backgroundLabel);
//}
 
void MainWindow::DrawStop(const Stop& stop, QVBoxLayout* layout) {
    if (!stop.name.empty()) {
        QLabel* background = new QLabel(ui->scrollAreaWidgetContents_2);
        background->setStyleSheet("background-color: #F8F8F8;");
        background->setFixedSize(589, 88);
        background->setAlignment(Qt::AlignCenter);

        QLabel* stop_name = new QLabel(QString::fromStdString(stop.name), background);
        stop_name->setStyleSheet("color: #2E1C0C; font: 700 16pt 'JetBrains Mono';");
        stop_name->setFixedSize(420, 24);
        stop_name->setAlignment(Qt::AlignLeft);
        stop_name->move(17, 17);

        QLabel* latitude = new QLabel("Широта: " + QString::number(stop.coords.lat), background);
        latitude->setStyleSheet("color: #2E1C0C; font: 500 11pt 'JetBrains Mono';");
        latitude->setFixedSize(176, 17);
        latitude->setAlignment(Qt::AlignLeft);
        latitude->move(17, 55);

        QLabel* longitude = new QLabel("Долгота: " + QString::number(stop.coords.lng), background);
        longitude->setStyleSheet("color: #2E1C0C; font: 500 11pt 'JetBrains Mono';");
        longitude->setFixedSize(176, 17);
        longitude->setAlignment(Qt::AlignLeft);
        longitude->move(209, 55);

        QPushButton* select_ = new QPushButton("&выбрать", background);
        select_->setStyleSheet("color: #2E1C0C; text-decoration: underline; font: 500 11pt 'JetBrains Mono'; background-color: transparent; text-align: right;");
        select_->setFixedSize(124, 17);
        select_->move(448, 17);

        layout->addWidget(background);
    }
}

void MainWindow::on_search_stop_clicked() {
    if (!database_is_updated) {
        QLayout* existingLayout = ui->scrollAreaWidgetContents_2->layout();
        if (existingLayout != nullptr) {
            QLayoutItem* item;
            while ((item = existingLayout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
            delete existingLayout;
        }

        QVBoxLayout* layout = new QVBoxLayout(ui->scrollAreaWidgetContents_2);
        layout->setAlignment(Qt::AlignTop);

        for (const auto& stop : ComputeTfIdfs(transport_catalogue_.GetSortedStops(), ui->lineEdit_find_stopname->text().toStdString())) {
            DrawStop(stop, layout);  
        }
        layout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

        ui->scrollAreaWidgetContents_2->setLayout(layout);
    }
}

void MainWindow::on_connect_to_db_clicked() {
    auto list = ui->stackedWidget->widget(0);
    QString hostname = list->findChild<QLineEdit*>("lineEdit_hostname")->text();
    int port = list->findChild<QLineEdit*>("lineEdit_port")->text().toInt();
    QString dbname = list->findChild<QLineEdit*>("lineEdit_dbname")->text();
    QString username = list->findChild<QLineEdit*>("lineEdit_username")->text();
    QString password = list->findChild<QLineEdit*>("lineEdit_password")->text();

    db_manager_.UpdateConnection(hostname, port, dbname, username, password);

    if (!db_manager_.Open()) {
        QMessageBox::critical(this, "Ошибка", "Выполните подключение к базе данных.");
        return;
    }
    QMessageBox::information(this, "Подключено", "Соединение с базой данных установлено."); 
}

void MainWindow::on_connect_to_db_default_clicked()
{
    auto list = ui->stackedWidget->widget(0);
    list->findChild<QLineEdit*>("lineEdit_hostname")->setText("localhost");
    list->findChild<QLineEdit*>("lineEdit_port")->setText("5432");
    list->findChild<QLineEdit*>("lineEdit_dbname")->setText("transport_catalogue");
    list->findChild<QLineEdit*>("lineEdit_username")->setText("postgres");
    list->findChild<QLineEdit*>("lineEdit_password")->setText("89274800234Nn");
}

void MainWindow::OpenEditBusDialog(const std::shared_ptr<Bus>& bus)
{ 
    BusEditor* bus_editor = new BusEditor(this, &db_manager_, bus);
    bus_editor->show();
}

void MainWindow::transferCatalogueDataToDatabase(DatabaseManager& db) {
    for (const auto& [stop_name, stop_ptr] : transport_catalogue_.GetSortedStops()) {
        db.AddStop(QString::fromStdString(stop_ptr.name), stop_ptr.coords.lat, stop_ptr.coords.lng);

        for (const auto& [to_stop, distance] : transport_catalogue_.GetDistances()) {
            if (to_stop.first->name == stop_ptr.name) {
                db.AddDistance(
                    QString::fromStdString(stop_ptr.name),
                    QString::fromStdString(to_stop.second->name),
                    distance
                );
            }
        }
    }

    for (const auto& [bus_name, bus_ptr] : transport_catalogue_.GetSortedBuses()) {
        QStringList stops;
        for (const Stop* stop : bus_ptr.stops) {
            stops << QString::fromStdString(stop->name);
        }
        db.AddBus(
            QString::fromStdString(bus_ptr.name),
            stops,
            bus_ptr.is_roundtrip,
            bus_ptr.color_index,
            QString::fromStdString(BusTypeToString(bus_ptr.bus_type)),
            bus_ptr.capacity,
            bus_ptr.has_wifi,
            bus_ptr.has_sockets,
            bus_ptr.is_night,
            bus_ptr.is_available,
            bus_ptr.price
        );
    }
}