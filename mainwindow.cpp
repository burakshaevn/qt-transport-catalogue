#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , db_manager_()
    , transport_catalogue_(db_manager_)
{
    ui->setupUi(this);
    this->setFixedSize(1300, 760);

    ui->stackedWidget->setCurrentWidget(ui->database);

    SetLineEditSettings();
    SetLabelSettings();
}

MainWindow::~MainWindow() = default;   

void MainWindow::DisplayMapOnLabel(const QString& bus_name) {
    auto feedback = JsonToSVG(bus_name);

    if (std::holds_alternative<QString>(feedback)) {
        QLayout* layout = ui->widget_2->layout();
        if (layout != nullptr) {
            QLayoutItem* item;
            while ((item = layout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
            delete layout;
        }
        QGraphicsScene* scene = new QGraphicsScene(this);
        QGraphicsView* view = new QGraphicsView(scene, ui->widget_2);

        QGraphicsSvgItem* svgItem = new QGraphicsSvgItem(std::get<QString>(feedback));
        scene->addItem(svgItem);

        view->setSceneRect(svgItem->boundingRect());
        view->setRenderHint(QPainter::Antialiasing);
        view->setDragMode(QGraphicsView::ScrollHandDrag);
        view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

        // РЈСЃС‚Р°РЅР°РІР»РёРІР°РµРј СЂР°Р·РјРµСЂ РґР»СЏ QGraphicsView
        view->setFixedSize(1280, 716); // РЈСЃС‚Р°РЅРѕРІРёС‚Рµ РЅСѓР¶РЅС‹Р№ СЂР°Р·РјРµСЂ

        // РЈСЃС‚Р°РЅР°РІР»РёРІР°РµРј СЃС‚РёР»СЊ
        view->setStyleSheet("border: 0px solid;"); 

        // РЈСЃС‚Р°РЅР°РІР»РёРІР°РµРј РјР°СЃС€С‚Р°Р±РёСЂРѕРІР°РЅРёРµ, РµСЃР»Рё СЌС‚Рѕ РЅРµРѕР±С…РѕРґРёРјРѕ
        qreal scaleFactor = 0.7;// 1.5;
        view->scale(scaleFactor, scaleFactor);
        view->move(10, 8);
        // РћС‚РѕР±СЂР°Р¶Р°РµРј QGraphicsView РІРЅСѓС‚СЂРё QLabel
        ui->widget_2->setLayout(new QVBoxLayout);
        ui->widget_2->layout()->addWidget(view);
    }
    else if (std::holds_alternative<std::runtime_error>(feedback)) {
        qDebug() << "РќРµ СѓРґР°Р»РѕСЃСЊ РѕС‚РєСЂС‹С‚СЊ С„Р°Р№Р» РґР»СЏ Р·Р°РїРёСЃРё.";
    }
}

// Р¤СѓРЅРєС†РёСЏ РґР»СЏ РїР°СЂСЃРёРЅРіР° СЃС‚СЂРѕРєРё Рё РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёСЏ РµС‘ РІ QVariantList
QVariantList ParseArrayString(const QString& array_str) {
    QVariantList result;

    // РЈР±РёСЂР°РµРј С„РёРіСѓСЂРЅС‹Рµ СЃРєРѕР±РєРё
    QString clean_str = array_str.mid(1, array_str.size() - 2); // РЈР±РёСЂР°РµРј РїРµСЂРІС‹Рµ Рё РїРѕСЃР»РµРґРЅРёРµ СЃРєРѕР±РєРё

    // Р?СЃРїРѕР»СЊР·СѓРµРј СЂРµРіСѓР»СЏСЂРЅРѕРµ РІС‹СЂР°Р¶РµРЅРёРµ РґР»СЏ СЂР°Р·РґРµР»РµРЅРёСЏ СЌР»РµРјРµРЅС‚РѕРІ
    QStringList elements = clean_str.split(QRegularExpression(QString("\\s*,\\s*")));

    // РџСЂРµРѕР±СЂР°Р·СѓРµРј СЌР»РµРјРµРЅС‚С‹ РІ С‡РёСЃР»Р° Рё РґРѕР±Р°РІР»СЏРµРј РІ СЂРµР·СѓР»СЊС‚Р°С‚
    for (const QString& element : elements) {
        result.push_back(element.toDouble());
    }

    return result;
}

MainWindow::Value MainWindow::JsonToSVG(const QString& bus_name) {
    QString appDir = QCoreApplication::applicationDirPath();

    json::Dict render_settings;

    QSqlQuery query_render = db_manager_.ExecuteSelectQuery(
        QString("SELECT * FROM render_settings;")
    );

    if (query_render.next()) {
        // Р?Р·РІР»РµС‡РµРЅРёРµ РѕР±С‹С‡РЅС‹С… Р·РЅР°С‡РµРЅРёР№
        render_settings["bus_label_font_size"] = json::Node(query_render.value("bus_label_font_size").toInt());

        // Р?Р·РІР»РµС‡РµРЅРёРµ РјР°СЃСЃРёРІР° bus_label_offset С‡РµСЂРµР· СЃС‚СЂРѕРєСѓ Рё РїР°СЂСЃРёРЅРі
        QString bus_label_offset_str = query_render.value("bus_label_offset").toString();
        QVariantList bus_label_offset_list = ParseArrayString(bus_label_offset_str);
        json::Array bus_label_offset;
        bus_label_offset.push_back(json::Node(bus_label_offset_list[0].toDouble()));
        bus_label_offset.push_back(json::Node(bus_label_offset_list[1].toDouble()));
        render_settings["bus_label_offset"] = bus_label_offset;

        // Р?Р·РІР»РµС‡РµРЅРёРµ stop_label_font_size
        render_settings["stop_label_font_size"] = json::Node(query_render.value("stop_label_font_size").toInt());

        // Р?Р·РІР»РµС‡РµРЅРёРµ РјР°СЃСЃРёРІР° stop_label_offset С‡РµСЂРµР· СЃС‚СЂРѕРєСѓ Рё РїР°СЂСЃРёРЅРі
        QString stop_label_offset_str = query_render.value("stop_label_offset").toString();
        QVariantList stop_label_offset_list = ParseArrayString(stop_label_offset_str);
        json::Array stop_label_offset;
        stop_label_offset.push_back(json::Node(stop_label_offset_list[0].toDouble()));
        stop_label_offset.push_back(json::Node(stop_label_offset_list[1].toDouble()));
        render_settings["stop_label_offset"] = stop_label_offset;

        // Р?Р·РІР»РµС‡РµРЅРёРµ stop_radius
        render_settings["stop_radius"] = json::Node(query_render.value("stop_radius").toInt());

        // Р?Р·РІР»РµС‡РµРЅРёРµ РјР°СЃСЃРёРІР° underlayer_color С‡РµСЂРµР· СЃС‚СЂРѕРєСѓ Рё РїР°СЂСЃРёРЅРі
        QString underlayer_color_str = query_render.value("underlayer_color").toString();
        QVariantList underlayer_color_list = ParseArrayString(underlayer_color_str);
        json::Array underlayer_color;
        underlayer_color.push_back(json::Node(underlayer_color_list[0].toInt()));
        underlayer_color.push_back(json::Node(underlayer_color_list[1].toInt()));
        underlayer_color.push_back(json::Node(underlayer_color_list[2].toInt()));
        underlayer_color.push_back(json::Node(underlayer_color_list[3].toDouble()));
        render_settings["underlayer_color"] = underlayer_color;

        // Р?Р·РІР»РµС‡РµРЅРёРµ РѕСЃС‚Р°Р»СЊРЅС‹С… РїР°СЂР°РјРµС‚СЂРѕРІ
        render_settings["underlayer_width"] = json::Node(query_render.value("underlayer_width").toDouble());
        render_settings["line_width"] = json::Node(query_render.value("line_width").toDouble());
        render_settings["padding"] = json::Node(query_render.value("padding").toInt());
        render_settings["height"] = json::Node(query_render.value("height").toInt());
        render_settings["width"] = json::Node(query_render.value("width").toInt());

        // Р?Р·РІР»РµС‡РµРЅРёРµ С†РІРµС‚РѕРІРѕР№ РїР°Р»РёС‚СЂС‹ РёР· Р±Р°Р·С‹ РґР°РЅРЅС‹С…
        QSqlQuery query_palette = db_manager_.ExecuteSelectQuery(
            QString("SELECT color FROM color_palette WHERE render_setting_id = %1;")
            .arg(query_render.value("id").toInt())
        );

        json::Array color_palette;
        
        // Р”РѕР±Р°РІР»СЏРµРј РїРµСЂРІС‹Р№ С†РІРµС‚ РєР°Рє В«С‚РµС…РЅРёС‡РµСЃРєРёР№В» вЂ” С‡С‘СЂРЅС‹Р№
        json::Array color;
        color.push_back(json::Node(0));
        color.push_back(json::Node(0));
        color.push_back(json::Node(0));
        color_palette.push_back(color);

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

        // Р“РµРЅРµСЂР°С†РёСЏ Рё СЃРѕС…СЂР°РЅРµРЅРёРµ SVG
        QString file_path = appDir + "/map.svg";
         QFile svg_file(file_path);

        if (!svg_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "The file could not be opened for writing.";
            throw std::runtime_error("The file could not be opened for writing.");
        }

        QTextStream out(&svg_file);
        std::ostringstream svg_data;
        rh.RenderMap(bus_name).Render(svg_data);
        out << QString::fromStdString(svg_data.str());

        svg_file.close();

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
            DrawBus(const_cast<Bus*>(bus_ptr), layout);
        }
        ui->scrollAreaWidgetContents->setLayout(layout);
    }
    else { 
        QMessageBox::critical(this, "Р”Р°РЅРЅС‹Рµ РЅРµ Р·Р°РіСЂСѓР¶РµРЅС‹", "Р’С‹РїРѕР»РЅРёС‚Рµ РїРѕРґРєР»СЋС‡РµРЅРёРµ Рє Р±Р°Р·Рµ РґР°РЅРЅС‹С…, С‡С‚РѕР±С‹ РѕС‚РѕР±СЂР°Р·РёС‚СЊ РґР°РЅРЅС‹Рµ.");
    }
}

void MainWindow::on_button_map_clicked()
{
    if (db_manager_.Open()) {
        ui->stackedWidget->setCurrentWidget(ui->map); 
        DisplayMapOnLabel("");
    }
    else {
        QMessageBox::critical(this, "Р”Р°РЅРЅС‹Рµ РЅРµ Р·Р°РіСЂСѓР¶РµРЅС‹", "Р’С‹РїРѕР»РЅРёС‚Рµ РїРѕРґРєР»СЋС‡РµРЅРёРµ Рє Р±Р°Р·Рµ РґР°РЅРЅС‹С…, С‡С‚РѕР±С‹ РѕС‚РѕР±СЂР°Р·РёС‚СЊ РґР°РЅРЅС‹Рµ.");
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
        size_t results_size{};
        for (auto& [stop_name, stop_ptr] : transport_catalogue_.GetSortedStops()) {
            DrawStop(const_cast<Stop*>(stop_ptr), layout);
            ++results_size;
        }
        ui->results_count->setText("Результатов: " + QString::number(results_size));
        ui->scrollAreaWidgetContents_2->setLayout(layout);
    }
    else {
        QMessageBox::critical(this, "Р”Р°РЅРЅС‹Рµ РЅРµ Р·Р°РіСЂСѓР¶РµРЅС‹", "Р’С‹РїРѕР»РЅРёС‚Рµ РїРѕРґРєР»СЋС‡РµРЅРёРµ Рє Р±Р°Р·Рµ РґР°РЅРЅС‹С…, С‡С‚РѕР±С‹ РѕС‚РѕР±СЂР°Р·РёС‚СЊ РґР°РЅРЅС‹Рµ.");
    }
}

void MainWindow::on_button_db_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->database);
}

void MainWindow::on_button_distances_clicked()
{
    if (db_manager_.Open()) {
        ui->stackedWidget->setCurrentWidget(ui->distances);
        LoadDistances();
    }
    else {
        QMessageBox::critical(this, "Р”Р°РЅРЅС‹Рµ РЅРµ Р·Р°РіСЂСѓР¶РµРЅС‹", "Р’С‹РїРѕР»РЅРёС‚Рµ РїРѕРґРєР»СЋС‡РµРЅРёРµ Рє Р±Р°Р·Рµ РґР°РЅРЅС‹С…, С‡С‚РѕР±С‹ РѕС‚РѕР±СЂР°Р·РёС‚СЊ РґР°РЅРЅС‹Рµ.");
    }
}

void MainWindow::SetLineEditSettings(){
    for (auto lineEdit : {
        ui->lineEdit_busname,
        ui->lineEdit_capacity,
        ui->lineEdit_price,
        ui->lineEdit_find_stopname
        }
        ) {
        lineEdit->setAlignment(Qt::AlignCenter);
    }
    ui->lineEdit_busname->setMaxLength(3);
    ui->lineEdit_stopname->setMaxLength(100);  

    auto database = ui->stackedWidget->widget(0); 
    database->findChild<QLineEdit*>("lineEdit_password")->setEchoMode(QLineEdit::Password);
}

void MainWindow::SetLabelSettings(){
    auto page_database = ui->stackedWidget->widget(0);
    page_database->findChild<QLineEdit*>("lineEdit_password")->setEchoMode(QLineEdit::Password);
}

void MainWindow::on_reset_all_filters_clicked()
{
    for(auto button_ : { ui->is_available, ui->is_unavailable }){
        button_->setAutoExclusive(false); // РћС‚РєР»СЋС‡Р°РµРј Р°РІС‚РѕСЌРєСЃРєР»СЋР·РёРІРЅРѕСЃС‚СЊ
        button_->setChecked(false);       // РЎР±СЂР°СЃС‹РІР°РµРј СЃРѕСЃС‚РѕСЏРЅРёРµ
        button_->setAutoExclusive(true);  // Р’РѕСЃСЃС‚Р°РЅР°РІР»РёРІР°РµРј Р°РІС‚РѕСЌРєСЃРєР»СЋР·РёРІРЅРѕСЃС‚СЊ
    }
    for (auto button_ : {
        ui->is_undefined,
        ui->is_autobus, 
        ui->is_trolleybus, 
        ui->is_electrobus, 
        ui->has_wifi, 
        ui->is_no_wifi,
        ui->has_sockets, 
        ui->is_no_sockets, 
        ui->is_night_route, 
        ui->is_day_route, 
        ui->is_roundtrip,
        ui->is_non_roundtrip,
        ui->sort_by_color_index
        })
    {
        button_->setChecked(false);
    }
    ui->lineEdit_busname->clear();
    ui->lineEdit_capacity->clear();
    ui->lineEdit_price->clear();
    ui->lineEdit_stopname->clear();
    
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
        DrawBus(const_cast<Bus*>(bus_ptr), layout);
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

    // РћРїСЂРµРґРµР»СЏРµРј, РєР°РєРѕР№ С‚РёРї РјР°СЂС€СЂСѓС‚Р° РёС‰РµС‚СЃСЏ
    std::set<BusType> selected_bus_types;
    if (ui->is_undefined->isChecked()) {
        selected_bus_types.insert(BusType::autobus);
        selected_bus_types.insert(BusType::electrobus);
        selected_bus_types.insert(BusType::trolleybus);
    }
    else if (ui->is_autobus->isChecked()) {
        selected_bus_types.insert(BusType::autobus);
    }
    else if (ui->is_electrobus->isChecked()) {
        selected_bus_types.insert(BusType::electrobus);
    }
    else if (ui->is_trolleybus->isChecked()) {
        selected_bus_types.insert(BusType::trolleybus);
    }

    std::optional<bool> has_wifi;
    if (ui->has_wifi->isChecked()) {
        has_wifi = true;
    }
    else if (ui->is_no_wifi->isChecked()) {
        has_wifi = false;
    }
    else {
        has_wifi = std::nullopt;
    }
    
    std::optional<bool> has_sockets;
    if (ui->has_sockets->isChecked()) {
        has_sockets = true;
    }
    else if (ui->is_no_sockets->isChecked()) {
        has_sockets = false;
    }
    else {
        has_sockets = std::nullopt;
    }
    
    std::optional<bool> is_night;
    if (ui->is_night_route->isChecked()) {
        is_night = true;
    }
    else if (ui->is_day_route->isChecked()) {
        is_night = false;
    }
    else {
        is_night = std::nullopt;
    }
    
    std::optional<bool> is_day;
    if (ui->is_day_route->isChecked()) {
        is_day = true;
    }
    else if (ui->is_day_route->isChecked()) {
        is_day = false;
    }
    else {
        is_day = std::nullopt;
    }
    
    DrawRelevantBuses(ui->lineEdit_busname->text(),
        { ui->lineEdit_stopname->text() },
        is_roundtrip,
        -1,
        selected_bus_types,
        static_cast<uint8_t>(ui->lineEdit_capacity->text().toInt()),
        has_wifi,
        has_sockets,
        is_night, 
        is_day, 
        is_available,
        static_cast<uint8_t>(ui->lineEdit_price->text().toInt()),
        ui->sort_by_color_index->isChecked()
    );
}

void MainWindow::DrawRelevantBuses(
    const std::optional<QStringView> name,
    const std::optional<QStringView> desired_stop,
    const std::optional<bool> is_roundtrip,
    const std::optional<size_t> color_index,
    const std::optional<std::set<BusType>>& bus_types,
    const std::optional<uint8_t> capacity,
    const std::optional<bool> has_wifi,
    const std::optional<bool> has_sockets,
    const std::optional<bool> is_night,
    const std::optional<bool> is_day,
    const std::optional<bool> is_available,
    const std::optional<uint8_t> price,
    const std::optional<bool> sort_by_color_index) {

    // РћС‡РёС‰Р°РµРј СЃСѓС‰РµСЃС‚РІСѓСЋС‰РёРµ РІРёРґР¶РµС‚С‹ РІ scrollArea
    QLayout* existingLayout = ui->scrollAreaWidgetContents->layout();
    if (existingLayout != nullptr) {
        QLayoutItem* item;
        while ((item = existingLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete existingLayout;
    }

    // Р¤СѓРЅРєС†РёСЏ РґР»СЏ РїРѕРґСЃС‡С‘С‚Р° СЂРµР»РµРІР°РЅС‚РЅРѕСЃС‚Рё РґР»СЏ РєР°Р¶РґРѕРіРѕ Р°РІС‚РѕР±СѓСЃР°
    auto compute_tf_idf_for_bus = [&](const Bus*& bus) -> double {
        int count_term_in_bus = 0;
        int total_terms = 0; // РљРѕР»РёС‡РµСЃС‚РІРѕ РїР°СЂР°РјРµС‚СЂРѕРІ, РєРѕС‚РѕСЂС‹Рµ СѓС‡РёС‚С‹РІР°СЋС‚СЃСЏ

        if (!name.has_value()) {
            ++total_terms;
            if (bus->name == name) {
                ++count_term_in_bus;
            }
        }
        if (!desired_stop.has_value()) {
            ++total_terms;
            for (const auto& stop : bus->stops) {
                if (stop->name == desired_stop) {
                    ++count_term_in_bus;
                    break;
                }
            }
        }
        if (capacity.has_value()) { // РџСЂРѕРІРµСЂРєР°, РµСЃР»Рё РІРјРµСЃС‚РёРјРѕСЃС‚СЊ СѓРєР°Р·Р°РЅР°
            ++total_terms;
            if (bus->capacity == capacity) {
                ++count_term_in_bus;
            }
        }
        if (price.has_value()) { // РџСЂРѕРІРµСЂРєР°, РµСЃР»Рё С†РµРЅР° СѓРєР°Р·Р°РЅР°
            ++total_terms;
            if (bus->price == price) {
                ++count_term_in_bus;
            }
        }
        if (has_wifi.has_value()) { // РџСЂРѕРІРµСЂСЏРµРј С‚РѕР»СЊРєРѕ РµСЃР»Рё РїРѕР»СЊР·РѕРІР°С‚РµР»СЊ РІС‹Р±СЂР°Р» Wi-Fi
            ++total_terms;
            if (bus->has_wifi == has_wifi) {
                ++count_term_in_bus;
            }
        }
        if (has_sockets.has_value()) { // РџСЂРѕРІРµСЂРєР° РЅР°Р»РёС‡РёСЏ СЂРѕР·РµС‚РѕРє
            ++total_terms;
            if (bus->has_sockets == has_sockets) {
                ++count_term_in_bus;
            }
        }
        if (is_night.has_value()) { // РџСЂРѕРІРµСЂРєР°, РЅРѕС‡РЅРѕР№ РјР°СЂС€СЂСѓС‚ РёР»Рё РЅРµС‚
            ++total_terms;
            if (bus->is_night == is_night) {
                ++count_term_in_bus;
            }
        }
        if (is_day.has_value()) { // РџСЂРѕРІРµСЂРєР°, РЅРѕС‡РЅРѕР№ РјР°СЂС€СЂСѓС‚ РёР»Рё РЅРµС‚
            ++total_terms;
            if (bus->is_day == is_day) {
                ++count_term_in_bus;
            }
        }
        if (is_roundtrip.has_value()) { // РџСЂРѕРІРµСЂРєР°, РєРѕР»СЊС†РµРІРѕР№ РёР»Рё РЅРµС‚
            ++total_terms;
            if (bus->is_roundtrip == is_roundtrip) {
                ++count_term_in_bus;
            }
        }

        // РџСЂРѕРІРµСЂРєР° РґРѕСЃС‚СѓРїРЅРѕСЃС‚Рё
        if (is_available.has_value()) {
            ++total_terms;
            if (bus->is_available == is_available) {
                ++count_term_in_bus;
            }
        }

        // РџСЂРѕРІРµСЂСЏРµРј, Р·Р°РґР°РЅС‹ Р»Рё С‚РёРїС‹ Р°РІС‚РѕР±СѓСЃРѕРІ
        if (!bus_types.has_value()) {
            ++total_terms;
            if (bus_types.value().find(bus->bus_type) != bus_types.value().end()) {
                ++count_term_in_bus;
            }
        }

        // Р•СЃР»Рё РїР°СЂР°РјРµС‚СЂС‹ РЅРµ Р·Р°РґР°РЅС‹, РёРіРЅРѕСЂРёСЂСѓРµРј СЌС‚РѕС‚ Р°РІС‚РѕР±СѓСЃ
        if (total_terms == 0) {
            return 0.0;
        }

        // TF: РѕС‚РЅРѕС€РµРЅРёРµ СЃРѕРІРїР°РґРµРЅРёР№ Рє РѕР±С‰РµРјСѓ С‡РёСЃР»Сѓ РїСЂРѕРІРµСЂСЏРµРјС‹С… РїР°СЂР°РјРµС‚СЂРѕРІ
        return (static_cast<double>(count_term_in_bus) / total_terms) * 1.;
        };

    QVBoxLayout* layout = new QVBoxLayout(ui->scrollAreaWidgetContents);
    layout->setAlignment(Qt::AlignTop);

    std::map<QStringView, const Bus*> buses = transport_catalogue_.GetSortedBuses();

    if (sort_by_color_index) {
        // РЎРѕСЂС‚РёСЂРѕРІРєР° РїРѕ color_index
        std::vector<const Bus*> buses_sorted_by_color;
        for (auto& [bus_name, bus] : buses) {
            buses_sorted_by_color.push_back(bus);
        }
        std::sort(buses_sorted_by_color.begin(), buses_sorted_by_color.end(), [](const Bus* a, const Bus* b) {
            return a->color_index < b->color_index;
        });

        // Р”РѕР±Р°РІР»СЏРµРј Р°РІС‚РѕР±СѓСЃС‹ РІ layout РІ РїРѕСЂСЏРґРєРµ color_index
        for (const Bus* bus : buses_sorted_by_color) {
            DrawBus(const_cast<Bus*>(bus), layout);
        }
    }
    else {
        std::multimap<double, const Bus*, std::greater<double>> relevant_buses;
        for (auto& [bus_name, bus] : buses) {
            double tf_idf = compute_tf_idf_for_bus(bus);
            if (tf_idf > renderer::EPSILON) {
                relevant_buses.emplace(tf_idf, bus);
            }
        }

        for (auto& [tf_idf, bus] : relevant_buses) {
            DrawBus(const_cast<Bus*>(bus), layout);
        }
    }

    ui->scrollAreaWidgetContents->setLayout(layout);
}

void MainWindow::DrawBus(Bus* bus, QVBoxLayout* layout) {
    // Лейбл, на котором будут располагаться блоки с информацией о маршруте
    QLabel* backgroundLabel = new QLabel(ui->scrollAreaWidgetContents);
    backgroundLabel->setStyleSheet("background-color: #FFFFFF; border-radius: 10px;");
    backgroundLabel->setFixedSize(631, 100);

    auto color = db_manager_.FindColor(bus->color_index);

    // Если цвет найден и является Rgb
    QString style_sheet_;
    if (color.has_value() && std::holds_alternative<svg::Rgb>(color.value())) {
        svg::Rgb rgbColor = std::get<svg::Rgb>(color.value());
        style_sheet_ = "color: rgb(" + QString::number(rgbColor.red) + ", " + QString::number(rgbColor.green) + ", " + QString::number(rgbColor.blue) + "); font: 500 20pt 'JetBrains Mono'; ";
    }
    else {
        // Если цвет не найден, задаём цвет по умолчанию
        style_sheet_ = "color: #2E1C0C; font: 500 20pt 'JetBrains Mono';";
    }

    // Применяем стиль к номеру автобуса
    QLabel* numberLabel = new QLabel(bus->name, backgroundLabel);
    numberLabel->setStyleSheet(style_sheet_);
    numberLabel->setFixedSize(70, 31);
    numberLabel->setAlignment(Qt::AlignLeft);
    numberLabel->move(12, 34);

    // информация о маршруте: тип автобуса, количество мест и остановок
    QString infoText;
    switch (bus->bus_type) {
    case BusType::autobus:
        infoText += "Автобус\n";
        break;
    case BusType::electrobus:
        infoText += "Электробус\n";
        break;
    case BusType::trolleybus:
        infoText += "Троллейбус\n";
        break;
    default:
        infoText += "Неопределён\n";
        break;
    }
    infoText += (QString::number(bus->capacity) + " мест\n");
    QString quantitative;
    auto size_ = bus->stops.size();

    if (size_ % 10 == 1 && size_ % 100 != 11) {
        quantitative = " остановка"; // 1, 21, 31, ...
    }
    else if (size_ % 10 >= 2 && size_ % 10 <= 4 && (size_ % 100 < 12 || size_ % 100 > 14)) {
        quantitative = " остановки"; // 2, 3, 4, 22, 23, 24, ...
    }
    else {
        quantitative = " остановок"; // 0, 5-9, 11-14, 15, ...
    }

    infoText += (QString::number(bus->stops.size()) + quantitative);

    QLabel* infoLabel = new QLabel(infoText, backgroundLabel);
    infoLabel->setStyleSheet("color: #2E1C0C; font: 500 10pt 'JetBrains Mono';");
    infoLabel->setFixedSize(138, 57);
    infoLabel->setAlignment(Qt::AlignLeft);
    infoLabel->move(84, 22);

    // кольцевой/некольцевой, длина и кривизна маршрута
    BusInfo bus_info = transport_catalogue_.GetBusInfo(bus);
    QString infoText2;
    infoText2 += (bus->is_roundtrip ? "Кольцевой\n" : "Некольцевой\n");
    infoText2 += ("Длина " + QString::number(bus_info.len) + " м\n");
    infoText2 += ("Кривизна " + QString::number(bus_info.curvature));

    QLabel* infoLabel2 = new QLabel(infoText2, backgroundLabel);
    infoLabel2->setStyleSheet("color: #2E1C0C; font: 500 10pt 'JetBrains Mono';");
    infoLabel2->setFixedSize(138, 57);
    infoLabel2->setAlignment(Qt::AlignLeft);
    infoLabel2->move(210, 22);

    // наличие wi-fi, розеток и ночной ли маршрут
    QString infoText3;
    infoText3 += (bus->has_wifi ? "Есть Wi-Fi\n" : "Нет Wi-Fi\n");
    infoText3 += (bus->has_sockets ? "Есть розетка\n" : "Нет розетки\n");
    if (!bus->is_night) {
        infoText3 += (bus->is_day ? "Дневной маршрут" : "Неопределён");
    }
    else {
        infoText3 += (bus->is_night ? "Ночной маршрут" : "Неопределён");
    }

    QLabel* infoLabel3 = new QLabel(infoText3, backgroundLabel);
    infoLabel3->setStyleSheet("color: #2E1C0C; font: 500 10pt 'JetBrains Mono';");
    infoLabel3->setFixedSize(153, 57);
    infoLabel3->setAlignment(Qt::AlignLeft);
    infoLabel3->move(370, 22);

    // цена
    QLabel* priceLabel = new QLabel(QString::number(bus->price) + " руб.", backgroundLabel);
    priceLabel->setStyleSheet("color: #2E1C0C; font: 500 15pt 'JetBrains Mono';");
    priceLabel->setFixedSize(125, 23);
    priceLabel->setAlignment(Qt::AlignRight);
    priceLabel->move(493, 63);

    // доступность
    QLabel* availableLabel = new QLabel(bus->is_available ? "Доступен" : "Недоступен", backgroundLabel);
    availableLabel->setStyleSheet(bus->is_available ? "color: #00BF63; font: 500 10pt 'JetBrains Mono';" : "color: #FF3131; font: 500 10pt 'JetBrains Mono';");
    availableLabel->setFixedSize(102, 17);
    availableLabel->setAlignment(Qt::AlignRight);
    availableLabel->move(514, 15);

    QPushButton* select_ = new QPushButton(backgroundLabel);
    select_->setIcon(QIcon(":/resources/edit.svg"));
    select_->setIconSize(QSize(20, 20));
    select_->setFixedSize(20, 20);
    select_->move(8, 3);
    select_->setStyleSheet("border-radius: 0px;");

    connect(select_, &QPushButton::clicked, [this, bus]() {
        OpenEditBusDialog(const_cast<Bus*>(bus));
    });

    layout->addWidget(backgroundLabel);
}

void MainWindow::EditStop(Stop* stop){
    StopEditor stopEditor(this, "Редактирование", "Сохранить");

    QLineEdit* stop_name_field = stopEditor.addField("Ред. Название:");
    QLineEdit* latitude_field = stopEditor.addField("Широта:");
    QLineEdit* longitude_field = stopEditor.addField("Долгота:");

    stop_name_field->setText(stop->name);
    latitude_field->setText(QString::number(stop->coords.lat));
    longitude_field->setText(QString::number(stop->coords.lng));

    connect(&stopEditor, &QDialog::finished, this, [&](){
        on_button_stops_clicked();
    });

    connect(&stopEditor, &StopEditor::fieldsEntered, this, [&]() {
        bool ok;
        double latitude = stopEditor.getFieldText(latitude_field).toDouble(&ok);
        if (!ok || latitude < 0) {
            QMessageBox::warning(&stopEditor, "Ошибка", "Введите корректное значение для широты.");
            return;
        }
        bool ok1;
        double longitude = stopEditor.getFieldText(longitude_field).toDouble(&ok1);
        if (!ok1 || longitude < 0) {
            QMessageBox::warning(&stopEditor, "Ошибка", "Введите корректное значение для долготы.");
            return;
        }

        QString stop_name = stopEditor.getFieldText(stop_name_field);

        if (db_manager_.UpdateStop(stop->name, stop_name, latitude, longitude)) {
            // transport_catalogue_.UpdateStop(stop->name, stop_name, latitude, longitude);
            transport_catalogue_.UpdateBuses();
            transport_catalogue_.UpdateBusnameToBus();
            transport_catalogue_.UpdateStops();
            transport_catalogue_.UpdateStopnameToStop();
            transport_catalogue_.UpdateStopBuses();
            transport_catalogue_.UpdateDistances();
            QMessageBox::information(this, "Успех", "Данные остановки обновлены.");
        } else {
            QMessageBox::critical(this, "Ошибка", "Произошла ошибка при обновлении.");
        }
    });

    stopEditor.exec();
}

void MainWindow::InfoStop(Stop* stop){
    StopEditor stopEditor(this, "Информация", "Закрыть");

    QLineEdit* stop_name_field = stopEditor.addField("Название:");
    QLineEdit* latitude_field = stopEditor.addField("Широта:");
    QLineEdit* longitude_field = stopEditor.addField("Долгота:");
    QLineEdit* count_buses_field = stopEditor.addField("Количество проходящих маршрутов:");

    stop_name_field->setReadOnly(true);
    latitude_field->setReadOnly(true);
    longitude_field->setReadOnly(true);
    count_buses_field->setReadOnly(true);

    stop_name_field->setText(stop->name);
    latitude_field->setText(QString::number(stop->coords.lat));
    longitude_field->setText(QString::number(stop->coords.lng));
    count_buses_field->setText(QString::number(transport_catalogue_.GetBusesForStop(stop->name).size()));

    stopEditor.exec();
}

void MainWindow::DeleteStop(Stop* stop){
    QMessageBox confirmBox;
    confirmBox.setWindowTitle("Подтверждение удаления");
    confirmBox.setText("Вы уверены, что хотите удалить остановку " + stop->name + "?");
    confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmBox.setDefaultButton(QMessageBox::No);

    int result = confirmBox.exec();
    if (result == QMessageBox::Yes) {
        if (db_manager_.DeleteStop(stop)) {
            //transport_catalogue_.DeleteStop(stop);
            transport_catalogue_.UpdateBuses();
            transport_catalogue_.UpdateBusnameToBus();
            transport_catalogue_.UpdateStops();
            transport_catalogue_.UpdateStopnameToStop();
            transport_catalogue_.UpdateStopBuses();
            transport_catalogue_.UpdateDistances();
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

            size_t results_size{};
            for(auto& stop : transport_catalogue_.GetSortedStops()){
                DrawStop(const_cast<Stop*>(stop.second), layout);
                ++results_size;
            }
            ui->results_count->setText("Результатов: " + QString::number(results_size));
            QMessageBox::information(this, "Удаление", "Остановка удалена.");
        }
        else {
            QMessageBox::warning(this, "Ошибка", "Не удалось удалить остановку.");
        }
    }
}

void MainWindow::DrawStop(Stop* stop, QVBoxLayout* layout) {
    if (!stop->name.isEmpty()) {
        QLabel* background = new QLabel(ui->scrollAreaWidgetContents_2);
        background->setStyleSheet("background-color: #F8F8F8; border-radius: 10px;");
        background->setFixedSize(589, 88);
        background->setAlignment(Qt::AlignCenter);

        QLineEdit* stop_name = new QLineEdit(stop->name, background);
        stop_name->setStyleSheet("color: #2E1C0C; font: 700 16pt 'JetBrains Mono';");
        stop_name->setFixedSize(420, 24);
        stop_name->setAlignment(Qt::AlignLeft);
        stop_name->move(17, 17);
        stop_name->setReadOnly(true);

        QLabel* location_image = new QLabel(background);
        QPixmap pixmap(":/resources/marker.svg");
        location_image->setPixmap(pixmap.scaled(17, 17, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        location_image->setFixedSize(17, 17);
        location_image->move(17, 55);

        QLineEdit* location = new QLineEdit("Координаты: " + QString::number(stop->coords.lat) + ", " + QString::number(stop->coords.lng), background);
        location->setStyleSheet("color: #2E1C0C; font: 500 11pt 'JetBrains Mono';");
        location->setFixedSize(412, 17);
        location->setAlignment(Qt::AlignLeft);
        location->move(45, 55);
        location->setReadOnly(true);

        QPushButton* info_ = new QPushButton(background);
        info_->setIcon(QIcon(":/resources/Information Circle Contained.svg"));
        info_->setStyleSheet("border-radius: 0px;");
        info_->setIconSize(QSize(17, 17));
        info_->setFixedSize(17, 17);
        info_->move(518, 7);

        connect(info_, &QPushButton::clicked, [this, stop]() {
            InfoStop(stop);
        });

        QPushButton* edit_ = new QPushButton(background);
        edit_->setIcon(QIcon(":/resources/edit.svg"));
        edit_->setStyleSheet("border-radius: 0px;");
        edit_->setIconSize(QSize(17, 17));
        edit_->setFixedSize(17, 17);
        edit_->move(541, 7);

        connect(edit_, &QPushButton::clicked, [this, stop]() {
            EditStop(stop);
        });

        QPushButton* delete_ = new QPushButton(background);
        delete_->setIcon(QIcon(":/resources/close.svg"));
        delete_->setStyleSheet("border-radius: 0px;");
        delete_->setIconSize(QSize(19, 19));
        delete_->setFixedSize(19, 19);
        delete_->move(563, 7);
        layout->addWidget(background);

        connect(delete_, &QPushButton::clicked, [this, stop]() {
            DeleteStop(stop);
        });
    }
}

void MainWindow::LoadDistances() {
    ui->tableWidgetDistances->clear();
    ui->tableWidgetDistances->setRowCount(0);
    ui->tableWidgetDistances->setColumnCount(3);

    QStringList headers = {"Откуда", "Куда", "Дистанция"};
    ui->tableWidgetDistances->setHorizontalHeaderLabels(headers);

    QSqlQuery query = db_manager_.ExecuteSelectQuery("SELECT d.id, s1.name AS from_stop_name, s2.name AS to_stop_name, d.distance FROM distances d JOIN stops s1 ON d.from_stop_id = s1.id JOIN stops s2 ON d.to_stop_id = s2.id");

    int row = 0;
    while (query.next()) {
        ui->tableWidgetDistances->insertRow(row);

        QString from_stop = query.value("from_stop_name").toString();
        QString to_stop = query.value("to_stop_name").toString();
        int distance = query.value("distance").toInt();

        ui->tableWidgetDistances->setItem(row, 0, new QTableWidgetItem(from_stop));
        ui->tableWidgetDistances->setItem(row, 1, new QTableWidgetItem(to_stop));
        ui->tableWidgetDistances->setItem(row, 2, new QTableWidgetItem(QString::number(distance)));

        row++;
    }

    ui->tableWidgetDistances->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidgetDistances->setColumnWidth(0, 280);
    ui->tableWidgetDistances->setColumnWidth(1, 280);
    ui->tableWidgetDistances->setColumnWidth(2, 280);
}

void MainWindow::on_stops_search_clicked() {
    QString term = ui->lineEdit_find_stopname->text();
    if (!term.isEmpty()) {

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
        size_t results_size{};
        for (const auto& stop : ComputeTfIdfs(transport_catalogue_.GetSortedStops(), term)) {
            DrawStop(stop, layout);
            ++results_size;
        }
        ui->results_count->setText("Результатов: " + QString::number(results_size));
        layout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

        ui->scrollAreaWidgetContents_2->setLayout(layout);
    }
    else{
        QMessageBox::warning(this, "Error", "Edit line is empty.");
    }
}

void MainWindow::on_stops_append_clicked() {
    StopEditor stopEditor(this, "Добавление остановки", "Добавить");

    QLineEdit* stop_name_field = stopEditor.addField("Название:");
    QLineEdit* latitude_field = stopEditor.addField("Широта:");
    QLineEdit* longitude_field = stopEditor.addField("Долгота:");

    connect(&stopEditor, &QDialog::finished, this, [&](){
        on_button_stops_clicked();
    });

    connect(&stopEditor, &StopEditor::fieldsEntered, this, [&]() {
        bool ok;
        double latitude = stopEditor.getFieldText(latitude_field).toDouble(&ok);
        if (!ok || latitude < 0) {
            QMessageBox::warning(&stopEditor, "Ошибка", "Введите корректное значение для широты.");
            return;
        }
        bool ok1;
        double longitude = stopEditor.getFieldText(longitude_field).toDouble(&ok1);
        if (!ok1 || longitude < 0) {
            QMessageBox::warning(&stopEditor, "Ошибка", "Введите корректное значение для долготы.");
            return;
        }

        QString stop_name = stopEditor.getFieldText(stop_name_field);

        transport_catalogue_.AddStop(stop_name, {latitude, longitude});
        if (db_manager_.AddStop(stop_name, latitude, longitude)) {
            QMessageBox::information(this, "Успех", "Остановка добавлена.");
        } else {
            QMessageBox::critical(this, "Ошибка", "Произошла ошибка при добавлении.");
        }
    });

    stopEditor.exec();
}

void MainWindow::on_search_distance_clicked()
{
    QString from = ui->lineEdit_from->text();
    QString to = ui->lineEdit_to->text();
    QString distance = ui->lineEdit_distance->text();

    QString queryStr = "SELECT s1.name AS from_stop_name, s2.name AS to_stop_name, d.distance "
                       "FROM distances d "
                       "JOIN stops s1 ON d.from_stop_id = s1.id "
                       "JOIN stops s2 ON d.to_stop_id = s2.id "
                       "WHERE 1=1";
    if (!from.isEmpty()) {
        queryStr += " AND s1.name LIKE '%" + from + "%'";
    }
    if (!to.isEmpty()) {
        queryStr += " AND s2.name LIKE '%" + to + "%'";
    }
    if (!distance.isEmpty()) {
        queryStr += " AND d.distance = " + distance;
    }

    QSqlQuery query = db_manager_.ExecuteSelectQuery(queryStr);

    ui->tableWidgetDistances->clearContents();
    ui->tableWidgetDistances->setRowCount(0);

    int row = 0;
    while (query.next()) {
        ui->tableWidgetDistances->insertRow(row);

        QString from_stop = query.value("from_stop_name").toString();
        QString to_stop = query.value("to_stop_name").toString();
        int dist = query.value("distance").toInt();

        ui->tableWidgetDistances->setItem(row, 0, new QTableWidgetItem(from_stop));
        ui->tableWidgetDistances->setItem(row, 1, new QTableWidgetItem(to_stop));
        ui->tableWidgetDistances->setItem(row, 2, new QTableWidgetItem(QString::number(dist)));

        ++row;
    }
    ui->tableWidgetDistances->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidgetDistances->setColumnWidth(0, 280);
    ui->tableWidgetDistances->setColumnWidth(1, 280);
    ui->tableWidgetDistances->setColumnWidth(2, 280);
}

void MainWindow::on_add_distance_clicked() {
    StopEditor stopEditor(this, "Добавление дистанции", "Добавить");

    QLineEdit* fromField = stopEditor.addField("Откуда:");
    QLineEdit* toField = stopEditor.addField("Куда:");
    QLineEdit* distanceField = stopEditor.addField("Расстояние:");

    connect(&stopEditor, &StopEditor::fieldsEntered, this, [&]() {
        bool ok;
        int distance = stopEditor.getFieldText(distanceField).toInt(&ok);
        if (!ok || distance < 0) {
            QMessageBox::warning(&stopEditor, "Ошибка", "Введите корректное расстояние.");
            return;
        }

        QString from = stopEditor.getFieldText(fromField);
        QString to = stopEditor.getFieldText(toField);

        if (!transport_catalogue_.GetDistance(from, to)) {
            if (db_manager_.StopIsExists(from)) {
                if (db_manager_.StopIsExists(to)) {
                    transport_catalogue_.SetDistance(from, to, distance);
                    if (db_manager_.AddDistance(from, to, distance)) {
                        LoadDistances();
                        QMessageBox::information(this, "", "Дистанция добавлена.");
                    } else {
                        QMessageBox::critical(this, "Ошибка", "Произошла ошибка при добавлении дистанции.");
                    }
                } else {
                    QMessageBox::warning(this, "Ошибка", "Остановки " + to + " не существует.");
                }
            }
            else {
                QMessageBox::warning(this, "Ошибка", "Остановки " + from + " не существует.");
            }
        }
        else{
            QMessageBox::warning(this, "", "Дистанция уже существует.");
        }
    });

    stopEditor.exec();
}

void MainWindow::on_delete_distance_clicked() {
    StopEditor stopEditor(this);

    QLineEdit* fromField = stopEditor.addField("Откуда:");
    QLineEdit* toField = stopEditor.addField("Куда:");

    connect(&stopEditor, &StopEditor::fieldsEntered, this, [&]() {
        QString from = stopEditor.getFieldText(fromField);
        QString to = stopEditor.getFieldText(toField);

        if (transport_catalogue_.GetDistance(from, to)) {
            if (db_manager_.StopIsExists(from)) {
                if (db_manager_.StopIsExists(to)) {
                    if (db_manager_.DeleteDistance(from, to)) {
                        LoadDistances();
                        QMessageBox::information(this, "", "Дистанция удалена.");
                    } else {
                        QMessageBox::critical(this, "Ошибка", "Произошла ошибка при удалении дистанции.");
                    }
                } else {
                    QMessageBox::warning(this, "Ошибка", "Остановки " + to + " не существует.");
                }
            }
            else {
                QMessageBox::warning(this, "Ошибка", "Остановки " + from + " не существует.");
            }
        }
        else{
            QMessageBox::warning(this, "", "Дистанция не существует.");
        }
    });

    stopEditor.exec();
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
        QMessageBox::critical(this, "Error", "Connect to the database.");
        return;
    }
    transport_catalogue_.UpdateBuses();
    transport_catalogue_.UpdateBusnameToBus();
    transport_catalogue_.UpdateStops();
    transport_catalogue_.UpdateStopnameToStop();
    transport_catalogue_.UpdateStopBuses();
    transport_catalogue_.UpdateDistances();

    QMessageBox::information(this, "Enabled", "The connection to the database is established.");
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

void MainWindow::OpenEditBusDialog(Bus* bus)
{
    BusEditor* bus_editor = new BusEditor(this, &db_manager_, &transport_catalogue_, bus);
      
    connect(bus_editor, &QDialog::finished, this, [this](int result) {
            Q_UNUSED(result);
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
            DrawBus(const_cast<Bus*>(bus_ptr), layout);
        }
        ui->scrollAreaWidgetContents->setLayout(layout);
        });

    bus_editor->show();
}

void MainWindow::on_lineEdit_find_stopname_textEdited(const QString &arg1)
{
    QIcon icon(":/resources/delete.svg");
    ui->stops_clear->setIcon(icon);
}

void MainWindow::on_stops_clear_clicked()
{
    ui->lineEdit_find_stopname->clear();
    ui->stops_clear->setIcon(QIcon());
    on_button_stops_clicked();
}
