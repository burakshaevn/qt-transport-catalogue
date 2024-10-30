#pragma once

#include <QMainWindow>
#include "ui_bus_editor.h" 
#include "database_manager.h"  
#include "mainwindow.h"

class BusEditor : public QMainWindow
{
	Q_OBJECT

public:
	BusEditor(QWidget *parent = nullptr, DatabaseManager* db_ = nullptr, TransportCatalogue* transport_catalogue_ = nullptr, Bus* bus_ = nullptr);
	~BusEditor();

private slots:
	void on_button_edit_clicked();
	void on_button_append_2_clicked();
	void on_button_settings_clicked();

	void on_button_append_clicked();
	void on_edit_page_save_clicked();
	void on_settings_save_clicked();

	void on_edit_page_append_stop_clicked();

private:
	Ui::BusEditorClass ui;
	DatabaseManager* db_manager;  
	Bus* current_bus;
	TransportCatalogue* transport_catalogue;

	// Хранит остановки current_bus.
	// Отдельный контейнер создаётся для того, чтобы при сохранении изменений мы могли выполнить синхронизацию 
	// с кэшем и базой данных.
	std::vector<const Stop*> stops_;

	void DisplayCurrentBusToEditPage();
	void DisplayCurrentBusToAppendPage(); 

	void FillComboBox(); 
	void SetLabelsAlign(); 
};
