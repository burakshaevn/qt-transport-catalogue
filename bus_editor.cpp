#include "bus_editor.h"

BusEditor::BusEditor(QWidget *parent, DatabaseManager* db_)
	: QMainWindow(parent)
	, db_manager(db_)
{
	ui.setupUi(this);
	this->setFixedSize(657, 671);
}

BusEditor::~BusEditor()
{}

void BusEditor::on_button_edit_clicked() {
	ui.stackedWidget->setCurrentWidget(ui.edit);
	if (db_manager->Open()) {

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

	}
	else {
		QMessageBox::critical(this, "Ошибка", "Выполните подключение к базе данных.");
	}
}