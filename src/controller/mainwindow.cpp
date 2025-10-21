#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , db_manager_()
    , transport_catalogue_(db_manager_)
{
    ui->setupUi(this);

    ui->tabWidget->setCurrentWidget(ui->tabWidgetDatabase);
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

        view->setFixedSize(1280, 716);

        view->setStyleSheet("border: 0px solid;"); 

        qreal scaleFactor = 0.7;// 1.5;
        view->scale(scaleFactor, scaleFactor);
        ui->widget_2->setLayout(new QVBoxLayout);
        ui->widget_2->layout()->addWidget(view);
    }
}

QVariantList ParseArrayString(const QString& array_str) {
    QVariantList result;

    QString clean_str = array_str.mid(1, array_str.size() - 2);

    QStringList elements = clean_str.split(QRegularExpression(QString("\\s*,\\s*")));

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
        render_settings["bus_label_font_size"] = json::Node(query_render.value("bus_label_font_size").toInt());

        QString bus_label_offset_str = query_render.value("bus_label_offset").toString();
        QVariantList bus_label_offset_list = ParseArrayString(bus_label_offset_str);
        json::Array bus_label_offset;
        bus_label_offset.push_back(json::Node(bus_label_offset_list[0].toDouble()));
        bus_label_offset.push_back(json::Node(bus_label_offset_list[1].toDouble()));
        render_settings["bus_label_offset"] = bus_label_offset;

        render_settings["stop_label_font_size"] = json::Node(query_render.value("stop_label_font_size").toInt());

        QString stop_label_offset_str = query_render.value("stop_label_offset").toString();
        QVariantList stop_label_offset_list = ParseArrayString(stop_label_offset_str);
        json::Array stop_label_offset;
        stop_label_offset.push_back(json::Node(stop_label_offset_list[0].toDouble()));
        stop_label_offset.push_back(json::Node(stop_label_offset_list[1].toDouble()));
        render_settings["stop_label_offset"] = stop_label_offset;

        render_settings["stop_radius"] = json::Node(query_render.value("stop_radius").toInt());

        QString underlayer_color_str = query_render.value("underlayer_color").toString();
        QVariantList underlayer_color_list = ParseArrayString(underlayer_color_str);
        json::Array underlayer_color;
        underlayer_color.push_back(json::Node(underlayer_color_list[0].toInt()));
        underlayer_color.push_back(json::Node(underlayer_color_list[1].toInt()));
        underlayer_color.push_back(json::Node(underlayer_color_list[2].toInt()));
        underlayer_color.push_back(json::Node(underlayer_color_list[3].toDouble()));
        render_settings["underlayer_color"] = underlayer_color;

        render_settings["underlayer_width"] = json::Node(query_render.value("underlayer_width").toDouble());
        render_settings["line_width"] = json::Node(query_render.value("line_width").toDouble());
        render_settings["padding"] = json::Node(query_render.value("padding").toInt());
        render_settings["height"] = json::Node(query_render.value("height").toInt());
        render_settings["width"] = json::Node(query_render.value("width").toInt());

        QSqlQuery query_palette = db_manager_.ExecuteSelectQuery(
            QString("SELECT * FROM public.color_palette ORDER BY id ASC "));

        json::Array color_palette;

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

void MainWindow::ClearScrollWidget(QLayout* layout){
    if (layout != nullptr) {
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete layout;
    }
}
void MainWindow::ClearSearchFiltres(){
    // for(auto button_ : { ui->is_available, ui->is_unavailable }){
    //     button_->setAutoExclusive(false);
    //     button_->setChecked(false);
    //     button_->setAutoExclusive(true);
    // }
    // for (auto button_ : {
    //          ui->is_undefined,
    //          ui->is_autobus,
    //          ui->is_trolleybus,
    //          ui->is_electrobus,
    //          ui->has_wifi,
    //          ui->is_no_wifi,
    //          ui->has_sockets,
    //          ui->is_no_sockets,
    //          ui->is_night_route,
    //          ui->is_day_route,
    //          ui->is_roundtrip,
    //          ui->is_non_roundtrip,
    //          ui->sort_by_color_index
    //      })
    // {
    //     button_->setChecked(false);
    // }
    // ui->lineEdit_busname->clear();
    // ui->lineEdit_capacity->clear();
    // ui->lineEdit_price->clear();
    // ui->lineEdit_stopname->clear();

    // ClearScrollWidget(ui->scrollArea_buses->layout());

    // QVBoxLayout* layout = new QVBoxLayout();
    // for (auto& [bus_name, bus_ptr] : transport_catalogue_.GetSortedBuses()) {
    //     DrawBus(bus_ptr, ui->show_colors->isChecked() ? true : false, layout);
    // }
    // ui->scrollArea_buses->setLayout(layout);
}

// void MainWindow::on_button_buses_clicked()
// {
//     if (db_manager_.Open()) {
//         ui->stackedWidget->setCurrentWidget(ui->buses);
//         DrawRelevantBuses();
//     }
//     else {
//         QMessageBox::critical(this, "Data not loaded", "Connect to the database to display data.");
//     }
// }

// void MainWindow::on_button_map_clicked()
// {
//     if (db_manager_.Open()) {
//         ui->stackedWidget_2->setCurrentWidget(ui->map);
//         DisplayMapOnLabel("");
//     }
//     else {
//         QMessageBox::critical(this, "Data not loaded", "Connect to the database to display data.");
//     }
// }

// void MainWindow::on_button_stops_clicked()
// {
//     if (db_manager_.Open()) {
//         ui->stackedWidget_2->setCurrentWidget(ui->stops);

//         ClearScrollWidget(ui->scrollArea_stops->layout());

//         QVBoxLayout* layout = new QVBoxLayout();
//         layout->setAlignment(Qt::AlignTop);
//         size_t results_size{};
//         for (auto& [stop_name, stop_ptr] : transport_catalogue_.GetSortedStops()) {
//             DrawStop(stop_ptr, layout);
//             ++results_size;
//         }
//         ui->scrollArea_stops->setLayout(layout);
//     }
//     else {
//         QMessageBox::critical(this, "Data not loaded", "Connect to the database to display data.");
//     }
// }

// void MainWindow::on_button_db_clicked()
// {
//     ui->stackedWidget->setCurrentWidget(ui->database);
// }

// void MainWindow::on_button_distances_clicked()
// {
//     if (db_manager_.Open()) {
//         ui->stackedWidget->setCurrentWidget(ui->distances);
//         LoadDistances();
//     }
//     else {
//         QMessageBox::critical(this, "Data not loaded", "Connect to the database to display data.");
//     }
// }

// void MainWindow::SetLabelSettings(){
//     auto page_database = ui->stackedWidget->widget(0);
//     page_database->findChild<QLineEdit*>("lineEdit_password")->setEchoMode(QLineEdit::Password);
// }

void MainWindow::on_reset_all_filters_clicked() {
    ClearSearchFiltres();
}

void MainWindow::on_search_bus_clicked() {    
    DrawRelevantBuses();
}

void MainWindow::DrawRelevantBuses() {
    // Получение параметров фильтрации из UI
    auto getOptionalText = [](QLineEdit* edit) {
        return edit->text().isEmpty() ? std::optional<QString>() : edit->text();
    };

    auto getOptionalInt = [](QLineEdit* edit) {
        bool ok;
        int value = edit->text().toInt(&ok);
        return ok ? std::optional<int>(value) : std::nullopt;
    };

    auto getOptionalBool = [](QComboBox* box) {
        switch(box->currentIndex()) {
        case 1: return std::optional<bool>(true);
        case 2: return std::optional<bool>(false);
        default: return std::optional<bool>();
        }
    };

    auto getEnumValue = [](QComboBox* box, auto defaultVal, auto... values) {
        int index = box->currentIndex();
        if (index > 0 && index <= sizeof...(values)) {
            return std::array{values...}[index-1];
        }
        return defaultVal;
    };

    // Получение параметров
    auto name = getOptionalText(ui->lineEdit_busname);
    auto stopname = getOptionalText(ui->lineEdit_stopname);
    auto price = getOptionalInt(ui->lineEdit_price);
    auto capacity = getOptionalInt(ui->lineEdit_capacity);

    auto bus_type = getEnumValue(ui->comboBoxType,
                                 BusProperties::Type::undefined,
                                 BusProperties::Type::autobus,
                                 BusProperties::Type::electrobus,
                                 BusProperties::Type::trolleybus);

    auto operating_time = getEnumValue(ui->comboBoxOperatingTime,
                                       BusProperties::OperatingTime::anytime,
                                       BusProperties::OperatingTime::daytime,
                                       BusProperties::OperatingTime::nighttime);

    auto direction = getEnumValue(ui->comboBoxDirection,
                                  BusProperties::Direction::undefined,
                                  BusProperties::Direction::circular,
                                  BusProperties::Direction::non_circular);

    auto has_wifi = getOptionalBool(ui->comboBoxHasWiFi);
    auto has_sockets = getOptionalBool(ui->comboBoxHasSockets);
    auto is_available = getOptionalBool(ui->comboBoxAvailable);

    bool show_colors = ui->comboBoxShowColors->currentIndex() == 1;
    bool sort_by_color = ui->comboBoxSortByColor->currentIndex() == 1;

    // Подготовка layout
    ClearScrollWidget(ui->scrollArea_buses->layout());
    QVBoxLayout* layout = new QVBoxLayout(ui->scrollArea_buses);
    layout->setAlignment(Qt::AlignTop);

    // Получение и фильтрация автобусов
    auto buses = transport_catalogue_.GetSortedBuses();

    if (!name && !price && !capacity && bus_type == BusProperties::Type::undefined &&
        !has_wifi && !has_sockets && operating_time == BusProperties::OperatingTime::anytime &&
        !is_available && direction == BusProperties::Direction::undefined && !sort_by_color) {
        // Если нет фильтров - отображаем все автобусы
        for (auto& [_, bus] : buses) {
            DrawBus(bus, show_colors, layout);
        }
    } else {
        // Фильтрация и сортировка
        std::vector<std::pair<double, Bus*>> relevant_buses;

        for (auto& [_, bus] : buses) {
            // Проверка соответствия фильтрам
            bool matches = true;

            if (name && bus->name != *name) matches = false;
            if (price && bus->price != *price) matches = false;
            if (capacity && bus->capacity != *capacity) matches = false;
            if (bus_type != BusProperties::Type::undefined && bus->type != bus_type) matches = false;
            if (has_wifi && bus->has_wifi != *has_wifi) matches = false;
            if (has_sockets && bus->has_sockets != *has_sockets) matches = false;
            if (is_available && bus->is_available != *is_available) matches = false;
            if (direction != BusProperties::Direction::undefined && bus->direction != direction) matches = false;
            if (operating_time != BusProperties::OperatingTime::anytime && bus->operating_time != operating_time) matches = false;

            if (matches) {
                double tf_idf = transport_catalogue_.ComputeTfIdfForBus(
                    bus, name, stopname, direction, bus_type, capacity,
                    has_wifi, has_sockets, operating_time, is_available, price
                    );

                if (tf_idf > renderer::EPSILON || sort_by_color) {
                    relevant_buses.emplace_back(tf_idf, bus);
                }
            }
        }

        // Сортировка
        if (sort_by_color) {
            std::sort(relevant_buses.begin(), relevant_buses.end(),
                      [](const auto& a, const auto& b) {
                          return (a.first > b.first) ||
                                 (a.first == b.first && a.second->color_index < b.second->color_index);
                      });
        }

        // Отрисовка
        for (auto& [_, bus] : relevant_buses) {
            DrawBus(bus, show_colors, layout);
        }
    }

    ui->scrollArea_buses->setLayout(layout);
}

// void MainWindow::DrawRelevantBuses() {
//     std::optional<QString> name = ui->lineEdit_busname->text().isEmpty()
//                                 ? std::nullopt
//                                 : std::make_optional(ui->lineEdit_busname->text());

//     std::optional<int> price = ui->lineEdit_price->text().isEmpty()
//                                ? std::nullopt
//                                : std::make_optional(ui->lineEdit_price->text().toInt());
//     std::optional<int> capacity = ui->lineEdit_capacity->text().isEmpty() ? std::nullopt : std::optional<int>(ui->lineEdit_capacity->text().toInt());

//     BusProperties::Type bus_type;
//     switch(ui->comboBoxType->currentIndex())
//     {
//     case 1:  bus_type = BusProperties::Type::autobus; break;
//     case 2:  bus_type = BusProperties::Type::electrobus; break;
//     case 3:  bus_type = BusProperties::Type::trolleybus; break;
//     default: bus_type = BusProperties::Type::undefined; break;
//     }

//     BusProperties::OperatingTime operating_time;
//     switch(ui->comboBoxOperatingTime->currentIndex())
//     {
//     case 1:  operating_time = BusProperties::OperatingTime::daytime; break;
//     case 2:  operating_time = BusProperties::OperatingTime::nighttime; break;
//     default: operating_time = BusProperties::OperatingTime::anytime; break;
//     }

//     BusProperties::Direction direction;
//     switch(ui->comboBoxDirection->currentIndex())
//     {
//     case 1:  direction = BusProperties::Direction::circular; break;
//     case 2:  direction = BusProperties::Direction::non_circular; break;
//     default: direction = BusProperties::Direction::undefined; break;
//     }

//     std::optional<bool> has_wifi;
//     switch(ui->comboBoxHasWiFi->currentIndex())
//     {
//     case 1:  has_wifi = std::optional<bool>(true); break;
//     case 2:  has_wifi = std::optional<bool>(false); break;
//     default: has_wifi = std::nullopt; break;
//     }

//     std::optional<bool> has_sockets;
//     switch(ui->comboBoxHasSockets->currentIndex())
//     {
//     case 1:  has_sockets = std::optional<bool>(true); break;
//     case 2:  has_sockets = std::optional<bool>(false); break;
//     default: has_sockets = std::nullopt; break;
//     }

//     std::optional<bool> is_available;
//     switch(ui->comboBoxAvailable->currentIndex())
//     {
//     case 1:  is_available = true; break;
//     case 2:  is_available = false; break;
//     default: is_available = std::nullopt; break;
//     }

//     std::optional<bool> sort_by_color_index;
//     switch(ui->comboBoxSortByColor->currentIndex())
//     {
//     case 1:  sort_by_color_index = std::optional<bool>(true); break;
//     default: sort_by_color_index = false; break;
//     }

//     std::optional<bool> show_colors;
//     switch(ui->comboBoxShowColors->currentIndex())
//     {
//     case 1:  show_colors = std::optional<bool>(true); break;
//     default: show_colors = false; break;
//     }

//     bool all_params_nullopt = !(name.has_value() || contains_stop.has_value() || direction == BusProperties::Direction::undefined ||
//         bus_type == BusProperties::Type::undefined || capacity.has_value() || has_wifi.has_value() ||
//         has_sockets.has_value() || operating_time != BusProperties::OperatingTime::anytime ||
//         is_available.has_value() || price.has_value() || sort_by_color_index.has_value());

//     ClearScrollWidget(ui->scrollArea_buses->layout());
//     QVBoxLayout* layout = new QVBoxLayout(ui->scrollArea_buses);
//     layout->setAlignment(Qt::AlignTop);
//     if (all_params_nullopt) {
//         for (auto& [bus_name, bus] : transport_catalogue_.GetSortedBuses()) {
//             DrawBus(bus, show_colors.value(), layout);
//         }
//     }
//     else {
//         auto sortByColorIndex = [](const std::pair<double, Bus*>& lhs, const std::pair<double, Bus*>& rhs) {
//             // Сначала по tf_idf в порядке убывания, затем по color_index в порядке возрастания
//             return (lhs.first > rhs.first) || (lhs.first == rhs.first && lhs.second->color_index < rhs.second->color_index);
//         };

//         std::vector<std::pair<double, Bus*>> relevant_buses;
//         for (auto& [bus_name, bus] : transport_catalogue_.GetSortedBuses()) {
//             double tf_idf = transport_catalogue_.ComputeTfIdfForBus(
//                 bus, name, desired_stop, is_roundtrip, bus_types, capacity,
//                 has_wifi, has_sockets, is_night, is_day, is_available, price
//             );

//             if (tf_idf > renderer::EPSILON || sort_by_color_index.value_or(false)) {
//                 relevant_buses.emplace_back(tf_idf, bus);
//             }
//         }

//         if (sort_by_color_index.value_or(false)) {
//             std::sort(relevant_buses.begin(), relevant_buses.end(), sortByColorIndex);
//         }

//         for (auto& [tf_idf, bus] : relevant_buses) {
//             DrawBus(bus, ui->show_colors->isChecked() ? true : false, layout);
//         }
//     }

//     ui->scrollArea_buses->setLayout(layout);
// }

// void MainWindow::DisplayBusCards()
// {
    // TODO: Пройтись по всем виджетам из раздела "Поиск" и если ни один виджет не применён, отрисовать все, иначе DrawRelevantBuses()
// }

// void MainWindow::DrawBus(Bus* bus, const bool show_color, QVBoxLayout* layout) {
//     QLabel* background = new QLabel(ui->scrollArea_buses);
//     background->setStyleSheet("background-color: #FFFFFF; border-radius: 22px;");
//     background->setFixedSize(499, 90);

//     QString style_sheet_;
//     if (show_color) {
//         auto color = db_manager_.FindColor(bus->color_index);
//         svg::Rgb rgbColor = std::get<svg::Rgb>(color.value());
//         qDebug() << QString::number(rgbColor.red) + ", " + QString::number(rgbColor.green) + ", " + QString::number(rgbColor.blue);
//         style_sheet_ = "color: rgb(" + QString::number(rgbColor.red) + ", " + QString::number(rgbColor.green) + ", " + QString::number(rgbColor.blue) + "); font: 400 20pt 'JetBrains Mono';";
//     }
//     else {
//         // Если цвет не найден, задаём цвет по умолчанию
//         style_sheet_ = "color: #2E1C0C; font: 500 20pt 'JetBrains Mono';";
//     }

//     QLabel* bus_name = new QLabel(bus->name, background);
//     bus_name->setStyleSheet(style_sheet_);
//     bus_name->setFixedSize(92, 32);
//     bus_name->setAlignment(Qt::AlignLeft);
//     bus_name->move(17, 17);

//     QString bus_type;
//     switch (bus->type) {
//     case BusProperties::Type::autobus:
//         bus_type += "Автобус";
//         break;
//     case BusProperties::Type::electrobus:
//         bus_type += "Электробус";
//         break;
//     case BusProperties::Type::trolleybus:
//         bus_type += "Троллейбус";
//         break;
//     default:
//         bus_type += "Неопределён";
//         break;
//     }
//     QLabel* bus_type_Label = new QLabel(bus_type, background);
//     bus_type_Label->setStyleSheet(QString("color: #2E1C0C; font: 500 11pt 'JetBrains Mono';"));
//     bus_type_Label->setFixedSize(92, 17);
//     bus_type_Label->setAlignment(Qt::AlignLeft);
//     bus_type_Label->move(17, 47);

//     QString infoText = (QString::number(bus->capacity) + " мест\n");
//     QString quantitative;
//     auto size_ = bus->stops.size();

//     if (size_ % 10 == 1 && size_ % 100 != 11) {
//         quantitative = " остановка"; // 1, 21, 31, ...
//     }
//     else if (size_ % 10 >= 2 && size_ % 10 <= 4 && (size_ % 100 < 12 || size_ % 100 > 14)) {
//         quantitative = " остановки"; // 2, 3, 4, 22, 23, 24, ...
//     }
//     else {
//         quantitative = " остановок"; // 0, 5-9, 11-14, 15, ...
//     }

//     infoText += (QString::number(bus->stops.size()) + quantitative);
//     infoText += bus->is_available ? "\nДоступен" : "\nНедоступен";

//     QLabel* infoLabel = new QLabel(infoText, background);
//     infoLabel->setStyleSheet("color: #2E1C0C; font: 500 11pt 'JetBrains Mono';");
//     infoLabel->setFixedSize(138, 57);
//     infoLabel->setAlignment(Qt::AlignLeft);
//     infoLabel->move(119, 17);

//     // Наличие Wi-Fi, розеток и время работы маршрута
//     QString infoText3;
//     infoText3 += (bus->has_wifi ? "Есть Wi-Fi\n" : "Нет Wi-Fi\n");
//     infoText3 += (bus->has_sockets ? "Есть розетка\n" : "Нет розетки\n");

//     switch(bus->operating_time)
//     {
//     case BusProperties::OperatingTime::anytime: infoText3 += "Круглосуточный"; break;
//     case BusProperties::OperatingTime::daytime: infoText3 += "Ночной"; break;
//     case BusProperties::OperatingTime::nighttime: infoText3 += "Дневной"; break;
//     default: infoText3 += "Неопределён"; break;
//     }

//     QLabel* infoLabel3 = new QLabel(infoText3, background);
//     infoLabel3->setStyleSheet("color: #2E1C0C; font: 500 11pt 'JetBrains Mono';");
//     infoLabel3->setFixedSize(138, 57);
//     infoLabel3->setAlignment(Qt::AlignLeft);
//     infoLabel3->move(268, 17);

//     // цена
//     QLabel* priceLabel = new QLabel(QString::number(bus->price) + " руб.", background);
//     priceLabel->setStyleSheet("color: #2E1C0C; font: 500 11pt 'JetBrains Mono';");
//     priceLabel->setFixedSize(153, 17);
//     priceLabel->setAlignment(Qt::AlignLeft);
//     priceLabel->move(417, 57);

//     QPushButton* info_ = new QPushButton(background);
//     info_->setIcon(QIcon(":/Information Circle Contained.svg"));
//     info_->setStyleSheet("border-radius: 0px;");
//     info_->setIconSize(QSize(17, 17));
//     info_->setFixedSize(17, 17);
//     info_->move(426, 10);

//     connect(info_, &QPushButton::clicked, [=]() {
//         InfoBus(bus);
//     });

//     QPushButton* select_ = new QPushButton(background);
//     select_->setIcon(QIcon(":/edit.svg"));
//     select_->setIconSize(QSize(17, 17));
//     select_->setFixedSize(17, 17);
//     select_->move(449, 10);
//     select_->setStyleSheet("border-radius: 0px;");

//     connect(select_, &QPushButton::clicked, [=]() {
//         EditBus(bus);
//     });

//     QPushButton* delete_ = new QPushButton(background);
//     delete_->setIcon(QIcon(":/close.svg"));
//     delete_->setIconSize(QSize(19, 19));
//     delete_->setFixedSize(19, 19);
//     delete_->move(471, 9);
//     delete_->setStyleSheet("border-radius: 0px;");

//     connect(background, &QObject::destroyed, this, []() {
//         std::cout << "Background уничтожен";
//         int x = 5;
//         });

//     connect(delete_, &QPushButton::clicked, [=]() {
//         DeleteBus(bus, layout, background);
//         });

//     layout->addWidget(background);
// }

void MainWindow::DrawBus(Bus* bus, const bool show_color, QVBoxLayout* layout) {
    // Создаем контейнер для карточки
    QWidget* cardWidget = new QWidget();
    cardWidget->setStyleSheet("background-color: #FFFFFF; border-radius: 10px;");
    cardWidget->setFixedWidth(499); // Фиксированная ширина, высота auto

    // Основной layout карточки
    QGridLayout* cardLayout = new QGridLayout(cardWidget);
    cardLayout->setContentsMargins(12, 12, 12, 12);
    cardLayout->setSpacing(8);

    // 1. Верхняя строка: номер маршрута + кнопки
    QHBoxLayout* topRowLayout = new QHBoxLayout();

    // Номер маршрута (с автоматическим размером)
    QLabel* busNumberLabel = new QLabel(bus->name);
    busNumberLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // Настройка цвета номера
    QString numberStyle = "font: 500 24pt 'JetBrains Mono';";
    if (show_color) {
        auto color = db_manager_.FindColor(bus->color_index);
        if (color.has_value() && std::holds_alternative<svg::Rgb>(color.value())) {
            auto rgb = std::get<svg::Rgb>(color.value());
            numberStyle += QString("color: rgb(%1,%2,%3);").arg(rgb.red).arg(rgb.green).arg(rgb.blue);
        }
    } else {
        numberStyle += "color: #2E1C0C;";
    }
    busNumberLabel->setStyleSheet(numberStyle);

    // Растягиваем номер маршрута
    topRowLayout->addWidget(busNumberLabel, 1);

    // Кнопки действий (верхний правый угол)
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(5);

    auto createButton = [](const QString& iconPath) {
        QPushButton* btn = new QPushButton();
        btn->setIcon(QIcon(iconPath));
        btn->setIconSize(QSize(16, 16));
        btn->setFixedSize(24, 24);
        btn->setStyleSheet(
            "QPushButton { border: none; background: transparent; }"
            "QPushButton:hover { background: #F0F0F0; border-radius: 4px; }"
        );
        return btn;
    };

    QPushButton* infoBtn = createButton(":/Information Circle Contained.svg");
    QPushButton* editBtn = createButton(":/edit.svg");
    QPushButton* deleteBtn = createButton(":/close.svg");

    buttonsLayout->addWidget(infoBtn);
    buttonsLayout->addWidget(editBtn);
    buttonsLayout->addWidget(deleteBtn);

    topRowLayout->addLayout(buttonsLayout);
    cardLayout->addLayout(topRowLayout, 0, 0, 1, 2);

    // 2. Основная информация (нижняя часть)
    QGridLayout* infoLayout = new QGridLayout();
    infoLayout->setVerticalSpacing(4);

    // Тип транспорта
    QString typeText;
    switch(bus->type) {
        case BusProperties::Type::autobus: typeText = "Автобус"; break;
        case BusProperties::Type::electrobus: typeText = "Электробус"; break;
        case BusProperties::Type::trolleybus: typeText = "Троллейбус"; break;
        default: typeText = "Неопределён"; break;
    }

    QLabel* typeLabel = new QLabel(typeText);
    typeLabel->setStyleSheet("color: #666666; font: 500 10pt 'JetBrains Mono';");
    infoLayout->addWidget(typeLabel, 0, 0);

    // Характеристики
    QLabel* specsLabel = new QLabel(
        QString("%1 мест · %2 ост.%3")
            .arg(bus->capacity)
            .arg(bus->stops.size())
            .arg(bus->is_available ? " · Доступен" : "")
    );
    specsLabel->setStyleSheet("color: #444444; font: 500 10pt 'JetBrains Mono';");
    infoLayout->addWidget(specsLabel, 0, 1);

    // Особенности
    QLabel* featuresLabel = new QLabel(
        QString("%1%2%3")
            .arg(bus->has_wifi ? "Wi-Fi " : "")
            .arg(bus->has_sockets ? "Розетки " : "")
            .arg([&](){
                switch(bus->operating_time) {
                    case BusProperties::OperatingTime::anytime: return "Круглосуточно";
                    case BusProperties::OperatingTime::daytime: return "Дневной";
                    case BusProperties::OperatingTime::nighttime: return "Ночной";
                    default: return "";
                }
            }())
    );
    featuresLabel->setStyleSheet("color: #444444; font: 500 10pt 'JetBrains Mono';");
    infoLayout->addWidget(featuresLabel, 1, 0, 1, 2);

    // Цена (выровнена по правому краю)
    QLabel* priceLabel = new QLabel(QString("%1 ₽").arg(bus->price));
    priceLabel->setStyleSheet("color: #2E1C0C; font: 500 12pt 'JetBrains Mono';");
    priceLabel->setAlignment(Qt::AlignRight);
    infoLayout->addWidget(priceLabel, 0, 2, 2, 1);

    cardLayout->addLayout(infoLayout, 1, 0, 1, 2);

    // Подключение сигналов
    connect(infoBtn, &QPushButton::clicked, [this, bus]() { InfoBus(bus); });
    connect(editBtn, &QPushButton::clicked, [this, bus]() { EditBus(bus); });
    connect(deleteBtn, &QPushButton::clicked, [this, bus, layout, cardWidget]() {
        // DeleteBus(bus, layout, cardWidget);
    });

    // Рассчитываем оптимальную высоту
    cardWidget->adjustSize();
    cardWidget->setFixedHeight(cardWidget->sizeHint().height());

    layout->addWidget(cardWidget);
}

void MainWindow::EditStop(const std::shared_ptr<const Stop>& stop){
    DialogEditor dialog_editor(this, "Редактирование", "Сохранить");

    QLineEdit* stop_name_field = dialog_editor.addField("Ред. Название:");
    QLineEdit* latitude_field = dialog_editor.addField("Широта:");
    QLineEdit* longitude_field = dialog_editor.addField("Долгота:");

    stop_name_field->setText(stop->name);
    latitude_field->setText(QString::number(stop->coords.lat));
    longitude_field->setText(QString::number(stop->coords.lng));

    connect(&dialog_editor, &QDialog::finished, this, [&](){
        DrawRelevantStops();
    });

    connect(&dialog_editor, &DialogEditor::fieldsEntered, this, [&]() {
        bool ok;
        double latitude = dialog_editor.getFieldText(latitude_field).toDouble(&ok);
        if (!ok || latitude < 0) {
            QMessageBox::warning(&dialog_editor, "Ошибка", "Введите корректное значение для широты.");
            return;
        }
        bool ok1;
        double longitude = dialog_editor.getFieldText(longitude_field).toDouble(&ok1);
        if (!ok1 || longitude < 0) {
            QMessageBox::warning(&dialog_editor, "Ошибка", "Введите корректное значение для долготы.");
            return;
        }

        QString stop_name = dialog_editor.getFieldText(stop_name_field);

        if (db_manager_.UpdateStop(stop->name, stop_name, latitude, longitude)) {
            transport_catalogue_.UpdateStops();
            transport_catalogue_.UpdateBuses();
            transport_catalogue_.UpdateDistances();
            QMessageBox::information(this, "Успех", "Данные остановки обновлены.");
        }
        else {
            QMessageBox::critical(this, "Ошибка", "Произошла ошибка при обновлении.");
        }
    });

    dialog_editor.exec();
}

void MainWindow::InfoStop(const std::shared_ptr<const Stop>& stop){
    DialogEditor dialog_editor(this, "Информация", "Закрыть");

    QLineEdit* stop_name_field = dialog_editor.addField("Название:");
    QLineEdit* latitude_field = dialog_editor.addField("Широта:");
    QLineEdit* longitude_field = dialog_editor.addField("Долгота:");
    QLineEdit* count_buses_field = dialog_editor.addField("Количество проходящих маршрутов:");

    stop_name_field->setReadOnly(true);
    latitude_field->setReadOnly(true);
    longitude_field->setReadOnly(true);
    count_buses_field->setReadOnly(true);

    stop_name_field->setText(stop->name);
    latitude_field->setText(QString::number(stop->coords.lat));
    longitude_field->setText(QString::number(stop->coords.lng));
    count_buses_field->setText(QString::number(transport_catalogue_.GetBusesForStop(stop->name).size()));

    dialog_editor.exec();
}

void MainWindow::DeleteStop(const std::shared_ptr<const Stop>& stop){
    QMessageBox confirmBox;
    confirmBox.setWindowTitle("Подтверждение удаления");
    confirmBox.setText("Вы уверены, что хотите удалить остановку " + stop->name + "?");
    confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmBox.setDefaultButton(QMessageBox::No);

    int result = confirmBox.exec();
    if (result == QMessageBox::Yes) {
        if (db_manager_.DeleteStop(stop.get())) {
            transport_catalogue_.DeleteStop(stop); 

            ClearScrollWidget(ui->scrollArea_stops->layout());

            QVBoxLayout* layout = new QVBoxLayout(ui->scrollArea_stops);
            layout->setAlignment(Qt::AlignTop);

            size_t results_size{};
            for(auto& stop : transport_catalogue_.GetSortedStops()){
                DrawStop(stop.second, layout);
                ++results_size;
            }
            QMessageBox::information(this, "Удаление", "Остановка удалена.");
        }
        else {
            QMessageBox::warning(this, "Ошибка", "Не удалось удалить остановку.");
        }
    }
}

void MainWindow::DrawStop(const std::shared_ptr<const Stop>& stop, QVBoxLayout* layout) {
    if (!stop->name.isEmpty()) {
        QLabel* background = new QLabel(ui->scrollArea_stops);
        background->setStyleSheet("background-color: #FFFFFF; border-radius: 22px;");
        background->setFixedSize(589, 88);
        background->setAlignment(Qt::AlignCenter);

        QLineEdit* stop_name = new QLineEdit(stop->name, background);
        stop_name->setStyleSheet("color: #2E1C0C; font: 700 16pt 'JetBrains Mono';");
        stop_name->setFixedSize(420, 24);
        stop_name->setAlignment(Qt::AlignLeft);
        stop_name->move(17, 17);
        stop_name->setReadOnly(true);

        QLabel* location_image = new QLabel(background);
        QPixmap pixmap(":/marker.svg");
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
        info_->setIcon(QIcon(":/Information Circle Contained.svg"));
        info_->setStyleSheet("border-radius: 0px;");
        info_->setIconSize(QSize(17, 17));
        info_->setFixedSize(17, 17);
        info_->move(518, 8);

        connect(info_, &QPushButton::clicked, [this, stop]() {
            InfoStop(stop);
        });

        QPushButton* edit_ = new QPushButton(background);
        edit_->setIcon(QIcon(":/edit.svg"));
        edit_->setStyleSheet("border-radius: 0px;");
        edit_->setIconSize(QSize(17, 17));
        edit_->setFixedSize(17, 17);
        edit_->move(541, 7);

        connect(edit_, &QPushButton::clicked, [this, stop]() {
            EditStop(stop);
        });

        QPushButton* delete_ = new QPushButton(background);
        delete_->setIcon(QIcon(":/close.svg"));
        delete_->setStyleSheet("border-radius: 0px;");
        delete_->setIconSize(QSize(19, 19));
        delete_->setFixedSize(19, 19);
        delete_->move(563, 6);
        layout->addWidget(background);

        connect(delete_, &QPushButton::clicked, [this, stop]() {
            DeleteStop(stop);
        });
    }
}
void MainWindow::DrawRelevantStops(){
    ClearScrollWidget(ui->scrollArea_stops->layout());
    QVBoxLayout* layout = new QVBoxLayout(ui->scrollArea_stops);
    layout->setAlignment(Qt::AlignTop);

    QString found_stop = ui->lineEdit_find_stopname->text();
    size_t results_size{};
    if (found_stop.isEmpty()){
        for(auto& stop : transport_catalogue_.GetSortedStops()){
            DrawStop(stop.second, layout);
            ++results_size;
        }
    }
    else{
        for (auto& stop : transport_catalogue_.ComputeTfIdfForStop(transport_catalogue_.GetSortedStops(), found_stop)) {
            DrawStop(stop, layout);
            ++results_size;
        }
    }
}

void MainWindow::LoadDistances() {
    ui->tableWidgetDistances->clear();
    ui->tableWidgetDistances->setRowCount(0);
    ui->tableWidgetDistances->setColumnCount(3);

    QStringList headers = {"Откуда (остановка)", "Куда (остановка)", "Дистанция (метры)"};
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
void MainWindow::DeleteDistance(){
    DialogEditor dialog_editor(this, "Удаление дистанции", "Удалить");

    QLineEdit* fromField = dialog_editor.addField("Откуда:");
    QLineEdit* toField = dialog_editor.addField("Куда:");

    connect(&dialog_editor, &DialogEditor::fieldsEntered, this, [&]() {
        QString from = dialog_editor.getFieldText(fromField);
        QString to = dialog_editor.getFieldText(toField);

        if (transport_catalogue_.GetDistance(from, to)) {
            if (db_manager_.StopIsExists(from)) {
                if (db_manager_.StopIsExists(to)) {
                    if (db_manager_.DeleteDistance(from, to)) {
                        LoadDistances();
                        DisplayMapOnLabel("");
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

    dialog_editor.exec();
}
void MainWindow::AddDistance() {
    DialogEditor stopEditor(this, "Добавление дистанции", "Добавить");

    QLineEdit* fromField = stopEditor.addField("Откуда:");
    QLineEdit* toField = stopEditor.addField("Куда:");
    QLineEdit* distanceField = stopEditor.addField("Расстояние:");

    connect(&stopEditor, &DialogEditor::fieldsEntered, this, [&]() {
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

void MainWindow::on_stops_search_clicked() {
    QString term = ui->lineEdit_find_stopname->text();
    if (!term.isEmpty()) {
        DrawRelevantStops();
    }
    else{
        QMessageBox::warning(this, "Error", "Edit line is empty.");
    }
}

void MainWindow::on_stops_append_clicked() {
    DialogEditor dialog_editor(this, "Добавление остановки", "Добавить");

    QLineEdit* stop_name_field = dialog_editor.addField("Название:");
    QLineEdit* latitude_field = dialog_editor.addField("Широта:");
    QLineEdit* longitude_field = dialog_editor.addField("Долгота:");

    connect(&dialog_editor, &QDialog::finished, this, [&](){
        DrawRelevantStops();
    });

    connect(&dialog_editor, &DialogEditor::fieldsEntered, this, [&]() {
        bool ok;
        double latitude = dialog_editor.getFieldText(latitude_field).toDouble(&ok);
        if (!ok || latitude < 0) {
            QMessageBox::warning(&dialog_editor, "Ошибка", "Введите корректное значение для широты.");
            return;
        }
        bool ok1;
        double longitude = dialog_editor.getFieldText(longitude_field).toDouble(&ok1);
        if (!ok1 || longitude < 0) {
            QMessageBox::warning(&dialog_editor, "Ошибка", "Введите корректное значение для долготы.");
            return;
        }

        QString stop_name = dialog_editor.getFieldText(stop_name_field);

        transport_catalogue_.AddStop(stop_name, {latitude, longitude});
        if (db_manager_.AddStop(stop_name, latitude, longitude)) {
            QMessageBox::information(this, "Успех", "Остановка добавлена.");
        } else {
            QMessageBox::critical(this, "Ошибка", "Произошла ошибка при добавлении.");
        }
    });

    dialog_editor.exec();
}

void MainWindow::on_search_distance_clicked() {
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
    AddDistance();
}
void MainWindow::on_delete_distance_clicked() {
    DeleteDistance();
}

// Выполняет подключение к базе данных.
void MainWindow::on_btn_connect_clicked() {

    // Получение значение из полей ввода
    auto getTextSafe = [](QWidget* parent, const char* name) -> QLineEdit* {
        if (auto le = parent->findChild<QLineEdit*>(name)) {
            return le;
        }
    };

    auto hostname_line_edit = getTextSafe(ui->tabWidgetDatabase, "le_host");
    auto port_line_edit = getTextSafe(ui->tabWidgetDatabase, "le_port");
    auto dbname_line_edit = getTextSafe(ui->tabWidgetDatabase, "le_dbName");
    auto username_line_edit = getTextSafe(ui->tabWidgetDatabase, "le_dbUsername");
    auto password_line_edit = getTextSafe(ui->tabWidgetDatabase, "le_dbPassword");

    if (!hostname_line_edit || !port_line_edit || !dbname_line_edit ||
        !username_line_edit || !password_line_edit) {
        QMessageBox::critical(this, "Error", "One or more input fields are missing.");
        return;
    }

    // Получение данных
    const QString hostname = hostname_line_edit->text();
    const int port = port_line_edit->text().toInt();
    const QString dbname = dbname_line_edit->text();
    const QString username = username_line_edit->text();
    const QString password = password_line_edit->text();

    // Обновление соединения
    db_manager_.UpdateConnection(hostname, port, dbname, username, password);

    try
    {
        if (!db_manager_.Open())
        {
            QMessageBox::critical(this, "Error", "Failed to connect to the database.");
            return;
        }

        // Обновление каталога
        transport_catalogue_.UpdateCatalogue();
        DisplayMapOnLabel("");
        ui->tabWidget->setCurrentWidget(ui->tabWidgetMap);

        QMessageBox::information(this, "Success", "The connection to the database is established.");
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, "Error", QString("An error occurred: %1").arg(e.what()));
    }
    catch (...)
    {
        QMessageBox::critical(this, "Error", "An unknown error occurred.");
    }
}

// Устанавливает значения по умолчанию для подключения к базе данных.
void MainWindow::on_btn_dataByDefault_clicked()
{
    auto setTextSafe = [](QWidget* parent, const char* name, const QString& text) {
        if (auto le = parent->findChild<QLineEdit*>(name)) {
            le->setText(text);
        }
    };

    setTextSafe(ui->tabWidgetDatabase, "le_host", "localhost");
    setTextSafe(ui->tabWidgetDatabase, "le_port", "5432");
    setTextSafe(ui->tabWidgetDatabase, "le_dbName", "transport_catalogue");
    setTextSafe(ui->tabWidgetDatabase, "le_dbUsername", "postgres");
    setTextSafe(ui->tabWidgetDatabase, "le_dbPassword", "89274800234Nn");
}

void MainWindow::EditBus(Bus* bus) {
    BusEditor* bus_editor = new BusEditor(this, &db_manager_, &transport_catalogue_, bus);

    connect(bus_editor, &QDialog::finished, this, [this](int result) {
        Q_UNUSED(result);

        ClearScrollWidget(ui->scrollArea_buses->layout());
        DrawRelevantBuses();
        });

    bus_editor->show();
}
void MainWindow::InfoBus(Bus* bus){
    DialogEditor dialog_editor(this, "Информация о маршруте", "Закрыть");

    QLineEdit* name = dialog_editor.addField("Название:");
    name->setText(bus->name);
    name->setReadOnly(true);

    QLineEdit* color_index = dialog_editor.addField("Цветовой индекс:");
    color_index->setText(QString::number(bus->color_index));
    color_index->setReadOnly(true);

    QLineEdit* bus_type = dialog_editor.addField("Тип маршрута:");
    bus_type->setText(EnumToString(bus->type));
    bus_type->setReadOnly(true);

    QLineEdit* capacity = dialog_editor.addField("Вместимость:");
    capacity->setText(QString::number(bus->capacity));
    capacity->setReadOnly(true);

    QLineEdit* stops_count = dialog_editor.addField("Количество остановок:");
    stops_count->setText(QString::number(bus->stops.size()));
    stops_count->setReadOnly(true);

    QLineEdit* is_roundtrip = dialog_editor.addField("Кольцевой (true/false):");
    // is_roundtrip->setText(bus->direction ? "true" : "false"); // TODO
    is_roundtrip->setReadOnly(true);

    BusInfo bus_info = transport_catalogue_.GetBusInfo(bus);
    QLineEdit* len = dialog_editor.addField("Длина маршрута (метры):");
    len->setText(QString::number(bus_info.len));
    len->setReadOnly(true);

    QLineEdit* curvature = dialog_editor.addField("Кривизна:");
    curvature->setText(QString::number(bus_info.curvature));
    curvature->setReadOnly(true);

    QLineEdit* has_wifi = dialog_editor.addField("Есть Wi-Fi (true/false):");
    has_wifi->setText(bus->has_wifi ? "true" : "false");
    has_wifi->setReadOnly(true);

    QLineEdit* has_sockets = dialog_editor.addField("Есть розетки (true/false):");
    has_sockets->setText(bus->has_sockets ? "true" : "false");
    has_sockets->setReadOnly(true);

    QLineEdit* is_day = dialog_editor.addField("Дневной маршрут (true/false):");
    // is_day->setText(bus->is_day ? "true" : "false"); // TODO
    is_day->setReadOnly(true);

    QLineEdit* is_night = dialog_editor.addField("Ночной маршрут (true/false):");
    // is_night->setText(bus->is_night ? "true" : "false"); // TODO
    is_night->setReadOnly(true);

    QLineEdit* price = dialog_editor.addField("Цена:");
    price->setText(QString::number(bus->price));
    price->setReadOnly(true);

    QLineEdit* available = dialog_editor.addField("Доступен (true/false):");
    available->setText(bus->is_available ? "true" : "false");
    available->setReadOnly(true);

    dialog_editor.exec();
}
void MainWindow::DeleteBus(Bus* bus, QVBoxLayout* layout, QLabel* background){
    QMessageBox confirmBox;
    confirmBox.setWindowTitle("Подтверждение удаления");
    confirmBox.setText("Вы уверены, что хотите удалить маршрут " + bus->name + "?");
    confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmBox.setDefaultButton(QMessageBox::No);

    int result = confirmBox.exec();
    if (result == QMessageBox::Yes) {
        // Удаляем виджет из макета
        layout->removeWidget(background); 
        delete background;               
        if (db_manager_.DeleteBus(bus)) {
            transport_catalogue_.DeleteBus(bus);
            // on_button_buses_clicked();
            QMessageBox::information(this, "Удаление", "Автобус успешно удалён.");
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось удалить автобус.");
        }
    }
}

void MainWindow::on_lineEdit_find_stopname_textEdited(const QString &arg1)
{
    QIcon icon(":/delete.svg");
    // ui->stops_clear->setIcon(icon);
}

void MainWindow::on_stops_clear_clicked()
{
    ui->lineEdit_find_stopname->clear();
    // ui->stops_clear->setIcon(QIcon());
    // on_button_stops_clicked();
}

void MainWindow::on_show_colors_clicked()
{

}

// void MainWindow::on_btn_sign_in_clicked()
// {
//     auto queryResult = db_manager_.ExecuteSelectQuery(QString("SELECT * FROM public.admins WHERE username = '%1';").arg(ui->lineEdit_login->text()));

//     if (queryResult.canConvert<QSqlQuery>()) {
//         QSqlQuery query = queryResult.value<QSqlQuery>();
//         if (query.next()) {
//             UserInfo user;
//             user.id_ = query.value("id").toInt();
//             user.full_name_ = query.value("full_name").toString();
//             user.email_ = query.value("email").toString();
//             user.password_ = query.value("password").toString();
//             user.role_ = StringToRole(query.value("role").toString());
//             if (user.password_ == ui->lineEdit_password->text()) {
//                 ui->lineEdit_login->clear();
//                 ui->lineEdit_password->clear();
//                 QMessageBox::information(this, "Авторизация", "Выполнена авторизация как администратор.");
//                 UpdateUser(user, this);

//                 table_ = std::make_unique<Table>(&db_manager_, user_.get(), nullptr);
//                 table_->BuildAdminTables();

//                 connect(table_.get(), &Table::Logout, this, &MainWindow::on_pushButton_logout_clicked);

//                 ui->stackedWidget->addWidget(table_.get());
//                 ui->stackedWidget->setCurrentWidget(table_.get());
//             }
//             else{
//                 QMessageBox::critical(this, "Авторизация", "Неверный логин или пароль.");
//             }
//         }
//         else {
//             auto queryResult = db_manager_.ExecuteSelectQuery(QString("SELECT * FROM public.clients WHERE email = '%1';").arg(ui->lineEdit_login->text()));

//             if (queryResult.canConvert<QSqlQuery>()) {
//                 QSqlQuery query = queryResult.value<QSqlQuery>();
//                 if (query.next()) {
//                     UserInfo user;
//                     user.id_ = query.value("id").toInt();
//                     user.full_name_ = query.value("first_name").toString();
//                     user.full_name_ += " " + query.value("last_name").toString();
//                     user.email_ = query.value("email").toString();
//                     user.password_ = query.value("password").toString();
//                     user.role_ = Role::User;
//                     user.purchased_cars_ = GetCars(user.id_);

//                     if (user.password_ == ui->lineEdit_password->text()) {
//                         ui->lineEdit_login->clear();
//                         ui->lineEdit_password->clear();
//                         QMessageBox::information(this, "Авторизация", "Выполнена авторизация как пользователь.");

//                         UpdateUser(user, this);
//                         ui->stackedWidget->setCurrentWidget(ui->main);

//                         SetupFloatingMenu();
//                         SetupSideMenu();
//                     }
//                     else{
//                         QMessageBox::critical(this, "Авторизация", "Неверный логин или пароль.");
//                     }
//                 }
//                 else {
//                     QMessageBox::critical(this, "Ошибка", "Пользователя с таким логином не существует.");
//                 }
//             }
//             else {
//                 QMessageBox::critical(this, "Ошибка в базе данных", queryResult.toString());
//             }
//         }
//     }
//     else {
//         QMessageBox::critical(this, "Ошибка в базе данных", queryResult.toString());
//     }
// }


void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    switch (index)
    {
    case 0: // Карта
        DisplayMapOnLabel("");
        break;
    case 1: // Дистанции
        break;
    case 2: // Автобусы
        DrawRelevantBuses();
        break;
    case 3: // Остановки
        DrawRelevantStops();
        break;
    case 4: // База данных
        qDebug() << "4";
        break;
    default:
        break;
    }
}
