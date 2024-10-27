#include "bus_editor.h"

BusEditor::BusEditor(QWidget *parent, DatabaseManager* db_, const std::shared_ptr<Bus>& bus_)
	: QMainWindow(parent)
	, db_manager(db_)
	, current_bus(bus_)
{
	ui.setupUi(this);
	this->setFixedSize(657, 671);

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
void BusEditor::on_button_append_clicked() {
	if (db_manager->Open()) {

	}
	else {
		QMessageBox::critical(this, "Ошибка", "Выполните подключение к базе данных.");
	}
}
void BusEditor::on_edit_save_clicked() {
	if (db_manager->Open()) {
		QString busname = ui.lineEdit_busname_2->text();
		QString bus_type = ui.comboBox_bustype_2->currentText();
		QString capacity = ui.lineEdit_capacity_2->text();
		QString is_roundtrip = ui.comboBox_is_roundtrip_2->currentText();
		QString color_index = ui.lineEdit_color_index_2->text();
		QString is_wifi = ui.comboBox_is_wifi_2->currentText();
		QString is_sockets = ui.comboBox_is_sockets_2->currentText();
		QString is_day_bus = ui.comboBox_is_day_bus_2->currentText();
		QString is_night_bus = ui.comboBox_is_night_bus_2->currentText();
		QString is_available = ui.comboBox_is_available_2->currentText();
		QString is_text = ui.lineEdit_price_2->text();
	}
	else {
		QMessageBox::critical(this, "Ошибка", "Выполните подключение к базе данных.");
	}
}

void BusEditor::DisplayCurrentBusToEditPage() {
	if (db_manager->Open()) { 
		QSqlQuery query = db_manager->ExecuteSelectQuery(
			QString("SELECT * FROM buses WHERE name = %1;").arg(QString::fromStdString(current_bus->name))
		);

		if (query.exec()) {
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

				// Устанавливаем значения в соответствующие поля
				ui.lineEdit_busname_2->setText(name);
				ui.comboBox_bustype_2->setCurrentText(busType);
				ui.lineEdit_capacity_2->setText(QString::number(capacity));
				ui.comboBox_is_roundtrip_2->setCurrentText(isRoundTrip ? "Да" : "Нет");
				ui.lineEdit_color_index_2->setText(QString::number(colorIndex));
				ui.comboBox_is_wifi_2->setCurrentText(hasWifi ? "Да" : "Нет");
				ui.comboBox_is_sockets_2->setCurrentText(hasSockets ? "Да" : "Нет");
				ui.comboBox_is_day_bus_2->setCurrentText(!isNightBus ? "Да" : "Нет"); // предполагаем, что если не ночной, то дневной
				ui.comboBox_is_night_bus_2->setCurrentText(isNightBus ? "Да" : "Нет");
				ui.comboBox_is_available_2->setCurrentText(isAvailable ? "Да" : "Нет");
				ui.lineEdit_price_2->setText(QString::number(price, 'f', 2)); // Форматирование цены
			}
		}
		else {
			// Обработка ошибки выполнения запроса
			QMessageBox::critical(this, "Ошибка", query.lastError().text());
		}
	}
	else {
		QMessageBox::critical(this, "Ошибка", "Выполните подключение к базе данных.");
	}
}
void BusEditor::DisplayCurrentBusToAppendPage() {
	if (db_manager->Open()) {

	}
	else {
		QMessageBox::critical(this, "Ошибка", "Выполните подключение к базе данных.");
	}
}