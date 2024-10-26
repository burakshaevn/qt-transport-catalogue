#pragma once

#include <QMainWindow>
#include "ui_bus_editor.h" 
#include "database_manager.h" 

class BusEditor : public QMainWindow
{
	Q_OBJECT

public:
	BusEditor(QWidget *parent = nullptr, DatabaseManager* db_ = nullptr);
	~BusEditor();

private slots:
	void on_button_edit_clicked();
	void on_button_append_2_clicked();
	void on_button_settings_clicked();

	void on_button_append_clicked();
	void on_edit_save_clicked();
	void on_settings_save_clicked();

private:
	Ui::BusEditorClass ui;
	DatabaseManager* db_manager;  
};
