#include "bus_editor.h"

BusEditor::BusEditor(QWidget* parent, DatabaseManager* db_, TransportCatalogue* transport_catalogue_, Bus* bus_)
	: QMainWindow(parent)
	, db_manager(db_)
	, transport_catalogue(transport_catalogue_)
	, current_bus(bus_)
	, stops_(bus_->stops)
{
	ui.setupUi(this);
	this->setFixedSize(657, 671);
	FillComboBox(); 
	SetLabelsAlign();
	DisplayCurrentBusToEditPage();
}

BusEditor::~BusEditor()
{}

void BusEditor::on_button_edit_clicked() {
	ui.stackedWidget->setCurrentWidget(ui.edit);
	if (db_manager->Open()) {
		DisplayCurrentBusToEditPage();
	}
	else {
		QMessageBox::critical(this, "Ошибка", "Выполните подключение к базе данных.");
	}
}
void BusEditor::on_button_append_2_clicked() {
	ui.stackedWidget->setCurrentWidget(ui.append);
	if (db_manager->Open()) {

	}
	else {
		QMessageBox::critical(this, "Ошибка", "Выполните подключение к базе данных.");
	}
}
void BusEditor::on_button_settings_clicked() {
	if (db_manager->Open()) {
		ui.stackedWidget->setCurrentWidget(ui.settings);

		QSqlQuery query = db_manager->ExecuteSelectQuery(QString("SELECT * FROM routing_settings;"));
		 
		if (query.next()) {  
			ui.lineEdit_bus_velocity->setText(query.value("bus_velocity").toString());
			ui.lineEdit_bus_wait_time->setText(query.value("bus_wait_time").toString());
		}
		else {
			QMessageBox::warning(this, "Предупреждение", "Данные не найдены.");
		}
	}
	else {
		QMessageBox::critical(this, "Ошибка", "Выполните подключение к базе данных.");
	}
}
void BusEditor::on_settings_save_clicked() {
	bool ok1, ok2;
	double busVelocity = ui.lineEdit_bus_velocity->text().toDouble(&ok1);
	int busWaitTime = ui.lineEdit_bus_wait_time->text().toInt(&ok2);

	if (db_manager->Open()) {
		if (ok1 && ok2) {
			if (db_manager->UpdateRoutingSettings(busVelocity, busWaitTime)) {
				QMessageBox::information(this, "Уведомление", "Данные обновлены.");
			}
		}
		else {
			QMessageBox::warning(this, "Ошибка", "Укажите корректные значения скорости и времени ожидания.");
		}
	}
	else {
		QMessageBox::critical(this, "Ошибка", "Выполните подключение к базе данных.");
	}
}
void BusEditor::on_edit_page_append_stop_clicked()
{
	QString stopname = ui.lineEdit_find_stopname->text();
	// Если остановка существует в базе данных, разрешаем добавить её в «Проходимые» автобусом
	if (const Stop* stop = transport_catalogue->FindStop(stopname); stop != nullptr) {
		stops_.push_back(stop);
		ui.lineEdit_find_stopname->clear();
		DisplayCurrentBusToEditPage();
		QMessageBox::information(this, "", QString("Остановка %1 добавлена в список проходимых остановок по маршруту.").arg(stopname));
	}
	else {
		QMessageBox::critical(this, "Ошибка", QString("Остановка %1 несуществует.").arg(stopname));
	}
}

void BusEditor::on_button_append_clicked() {
	if (db_manager->Open()) {
		// ...
	}
	else {
		QMessageBox::critical(this, "Ошибка", "Выполните подключение к базе данных.");
	}
}
void BusEditor::on_edit_page_save_clicked() {
	if (db_manager->Open()) {
		QString busname = ui.lineEdit_busname_2->text();
		QString bus_type = ui.comboBox_bustype_2->currentText();
		size_t capacity = static_cast<size_t>(ui.lineEdit_capacity_2->text().toInt());
		bool is_roundtrip = ui.comboBox_is_roundtrip_2->currentText() == "Да" ? true : false;
		int color_index = ui.lineEdit_color_index_2->text().toInt();
		bool is_wifi = ui.comboBox_is_wifi_2->currentText() == "Да" ? true : false;
		bool is_sockets = ui.comboBox_is_sockets_2->currentText() == "Да" ? true : false;
		bool is_day_bus = ui.comboBox_is_day_bus_2->currentText() == "Да" ? true : false;
		bool is_night_bus = ui.comboBox_is_night_bus_2->currentText() == "Да" ? true : false;
		bool is_available = ui.comboBox_is_available_2->currentText() == "Да" ? true : false;
		int is_text = ui.lineEdit_price_2->text().toInt();
		current_bus->stops.clear();
		current_bus->stops = std::move(stops_);
		db_manager->UpdateBus(busname, current_bus->stops, is_roundtrip, color_index, bus_type, capacity, is_wifi, is_sockets, is_night_bus, is_available, is_text);
	}
	else {
		QMessageBox::critical(this, "Ошибка", "Выполните подключение к базе данных.");
	}
}

void BusEditor::DisplayCurrentBusToEditPage() {
	if (db_manager->Open()) {
		QSqlQuery query = db_manager->ExecuteSelectQuery(
			QString("SELECT * FROM buses WHERE name = '%1';").arg(current_bus->name)
		);

		while (query.next()) {
			QString name = query.value("name").toString();
			QString busType = query.value("bus_type").toString();
			int capacity = query.value("capacity").toInt();
			bool isRoundTrip = query.value("is_roundtrip").toBool();
			int colorIndex = query.value("color_index").toInt();
			bool hasWifi = query.value("has_wifi").toBool();
			bool hasSockets = query.value("has_sockets").toBool();
			bool isNightBus = query.value("is_night").toBool();
			bool isAvailable = query.value("is_available").toBool();
			double price = query.value("price").toDouble();
			  
			ui.lineEdit_busname_2->setText(name);
			ui.comboBox_bustype_2->setCurrentText(busType);
			ui.lineEdit_capacity_2->setText(QString::number(capacity));
			ui.comboBox_is_roundtrip_2->setCurrentText(isRoundTrip ? "Да" : "Нет");
			ui.lineEdit_color_index_2->setText(QString::number(colorIndex));
			ui.comboBox_is_wifi_2->setCurrentText(hasWifi ? "Да" : "Нет");
			ui.comboBox_is_sockets_2->setCurrentText(hasSockets ? "Да" : "Нет");
			ui.comboBox_is_day_bus_2->setCurrentText(isNightBus ? "Да" : "Нет");
			ui.comboBox_is_night_bus_2->setCurrentText(isNightBus ? "Да" : "Нет");
			ui.comboBox_is_available_2->setCurrentText(isAvailable ? "Да" : "Нет");
			ui.lineEdit_price_2->setText(QString::number(price, 'f', 2));

			QLayout* existingLayout = ui.scrollAreaWidgetContents->layout();
			if (existingLayout != nullptr) {
				QLayoutItem* item;
				while ((item = existingLayout->takeAt(0)) != nullptr) {
					delete item->widget();
					delete item;
				}
				delete existingLayout;
			}

			QVBoxLayout* layout = new QVBoxLayout(ui.scrollAreaWidgetContents);
			layout->setAlignment(Qt::AlignTop);  
			
			for (size_t i = 0; i < stops_.size(); ++i) {
				if (!stops_[i]->name.isEmpty()) {
					QLabel* background = new QLabel(ui.scrollAreaWidgetContents);
					background->setStyleSheet("background-color: #F8F8F8;");
					background->setFixedSize(349, 17);

					QLabel* stop_name = new QLabel(stops_[i]->name, background);
					stop_name->setStyleSheet("color: #2E1C0C; font: 700 11t 'JetBrains Mono';");
					stop_name->setFixedSize(290, 17);
					stop_name->setAlignment(Qt::AlignLeft);
					stop_name->move(62, 0);

					/*QSqlQuery stop_position_query = db_manager->ExecuteSelectQuery(QString("select stops.name, bus_stops.stop_position from bus_stops left join buses ON bus_stops.bus_id = buses.id left join stops ON bus_stops.stop_id = stops.id where buses.name = '%1';")
						.arg(QString::fromStdString(current_bus->name)));*/

					//if (stop_position_query.next()) {
						QLabel* stop_position = new QLabel(QString::fromStdString(std::to_string(i + 1))/*stop_position_query.value(0).toString()*/, background);
						stop_position->setStyleSheet("color: #2E1C0C; font: 700 11t 'JetBrains Mono';");
						stop_position->setFixedSize(48, 17);
						stop_position->setAlignment(Qt::AlignLeft);
						stop_position->move(5, 0);

						QPushButton* delete_ = new QPushButton("✕", background);
						delete_->setFixedSize(17, 17);
						delete_->move(331, 0);
						delete_->setStyleSheet("border-radius: 0px; font: 9pt 'JetBrains Mono'; ");

						layout->addWidget(background);
					//}
				}
			}
		}
	}
	else {
		QMessageBox::critical(this, "Ошибка", "Выполните подключение к базе данных.");
	}
}
void BusEditor::DisplayCurrentBusToAppendPage() {
	if (db_manager->Open()) {
		// ...
	}
	else {
		QMessageBox::critical(this, "Ошибка", "Выполните подключение к базе данных.");
	}
}

void BusEditor::FillComboBox()
{
	// Page: edit; ComboBox: bus_type
	ui.comboBox_bustype_2->addItem("undefined");
	ui.comboBox_bustype_2->setCurrentText("undefined");
	ui.comboBox_bustype_2->addItem("autobus");
	ui.comboBox_bustype_2->addItem("electrobus");
	ui.comboBox_bustype_2->addItem("trolleybus");

	// Page: edit; ComboBox: is_roundtrip
	ui.comboBox_is_roundtrip_2->addItem("Не выбрано");
	ui.comboBox_is_roundtrip_2->setCurrentText("Не выбрано");
	ui.comboBox_is_roundtrip_2->addItem("Да");
	ui.comboBox_is_roundtrip_2->addItem("Нет");

	// Page: edit; ComboBox: is_wifi
	ui.comboBox_is_wifi_2->addItem("Не выбрано");
	ui.comboBox_is_wifi_2->setCurrentText("Не выбрано");
	ui.comboBox_is_wifi_2->addItem("Да");
	ui.comboBox_is_wifi_2->addItem("Нет");

	// Page: edit; ComboBox: is_sockets
	ui.comboBox_is_sockets_2->addItem("Не выбрано");
	ui.comboBox_is_sockets_2->setCurrentText("Не выбрано");
	ui.comboBox_is_sockets_2->addItem("Да");
	ui.comboBox_is_sockets_2->addItem("Нет");

	// Page: edit; ComboBox: is_day_bus
	ui.comboBox_is_day_bus_2->addItem("Не выбрано");
	ui.comboBox_is_day_bus_2->setCurrentText("Не выбрано");
	ui.comboBox_is_day_bus_2->addItem("Да");
	ui.comboBox_is_day_bus_2->addItem("Нет");

	// Page: edit; ComboBox: is_night_bus
	ui.comboBox_is_night_bus_2->addItem("Не выбрано");
	ui.comboBox_is_night_bus_2->setCurrentText("Не выбрано");
	ui.comboBox_is_night_bus_2->addItem("Да");
	ui.comboBox_is_night_bus_2->addItem("Нет");

	// Page: edit; ComboBox: is_available
	ui.comboBox_is_available_2->addItem("Не выбрано");
	ui.comboBox_is_available_2->setCurrentText("Не выбрано");
	ui.comboBox_is_available_2->addItem("Да");
	ui.comboBox_is_available_2->addItem("Нет");
}

void BusEditor::SetLabelsAlign()
{
	for (auto& obj : {
		ui.label_2,
		ui.label_38,
		ui.label_51,
		ui.label_52,
		ui.label_14,
		}) 
	{
		obj->setAlignment(Qt::AlignCenter);
	}  
} 
 