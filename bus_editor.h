#pragma once

#include <QMainWindow>
#include "ui_bus_editor.h"
#include "database_manager.h"
#include "mainwindow.h"
#include <QListWidget>
// #pragma once

// #ifndef STDAFX_H
// #define STDAFX_H

// #include "backend/include/json_reader.h"
// #include "backend/include/request_handler.h"
// #include "backend/include/transport_catalogue.h"
// #include "backend/include/transport_router.h"

// #include <fstream>
// #include <variant>

// #include <QDebug>
// #include <QDesktopServices>
// #include <QFile>
// #include <QFileDialog>
// #include <QGraphicsDropShadowEffect>
// #include <QGraphicsRectItem>
// #include <QGraphicsScene>
// #include <QGraphicsSvgItem>
// #include <QGraphicsView>
// #include <QLabel>
// #include <QPainter>
// #include <QPixmap>
// #include <QPushButton>
// #include <QRegularExpression>
// #include <QSvgRenderer>
// #include <QUrl>
// #include <QVBoxLayout>


// #endif // STDAFX_H


class BusEditor : public QDialog
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
    void on_edit_save_clicked();
	void on_settings_save_clicked();

    void on_edit_append_stop_clicked();
    void on_append_append_stop_clicked();

    void on_edit_delete_clicked();

private:
	Ui::BusEditorClass ui;
	// Указатель на текущий автобус, для которого открыто окно BusEditor
	Bus* current_bus;
	// Указатель на новый маршрут, который будет добавляться через окно «Добавление».
	// Изначально он будет nullptr. Как только открывается окно «Добавление», он
	// инициализируется new Bus(), куда впоследствии будет занесена информация из лейблов QLabel и QComboBox.
	std::unique_ptr<Bus> new_bus;
	DatabaseManager* db_manager;
	TransportCatalogue* transport_catalogue;

	// Хранит остановки current_bus.
	// Отдельный контейнер создаётся для того, чтобы при сохранении изменений мы могли выполнить синхронизацию 
	// с кэшем и базой данных.
	// Почему сразу не работать с current_bus->stops? Потому что пользователь может не прожать кнопку «Сохранить»,
	// что говорит нам об отмене изменений.
	std::vector<const Stop*> cache_stops_;
	std::vector<const Stop*> cache_new_bus_stops_; 

	void DisplayCurrentBusToEditPage(QListWidget* listWidgetStops);

    void FillComboBox();

	void SetupStopListWidget();

    // Функция для обновления порядка остановок после перемещения
	void UpdateStopOrder(); 

    // Функция для создания виджета остановки с меткой и кнопкой удаления
	QWidget* CreateStopWidget(const Stop* stop, std::vector<const Stop*>& cache_array, QListWidget* listWidgetStops);

    // Удаление остановки из `stops_` и интерфейса
    void on_edit_delete_stop_clicked(const Stop* stop, std::vector<const Stop*>& cache_array, QListWidget* listWidgetStops);

    // Функция для обработки изменения позиции остановки
    void on_stop_position_changed(int new_position, const Stop* stop, std::vector<const Stop*>& cache_array, QListWidget* listWidgetStops);
};
