#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "stdafx.h"
#include "database_manager.h" 
#include "bus_editor.h"
#include <algorithm>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

    using Value = std::variant<QString, std::runtime_error>;  

private slots:
    void on_button_buses_clicked();

    void on_button_map_clicked();

    void on_button_stops_clicked();

    void on_button_db_clicked();

    void on_reset_all_filters_clicked();

    void on_search_bus_clicked();

    void on_search_stop_clicked();

    void on_connect_to_db_clicked();

    void on_connect_to_db_default_clicked();

    void OpenEditBusDialog(Bus* bus);

private:
    Ui::MainWindow *ui;

    DatabaseManager db_manager_;

    bool database_is_updated{ false }; // флаг, отвечающий за состояние базы данных: были ли внесены правки, чтобы определить, нужно ли получать из неё данные повторно или это излишне

    TransportCatalogue transport_catalogue_;

    void SetLineEditSettings();
    void SetLabelSettings();

    void DisplayMapOnLabel(const QString& bus_name);

    Value JsonToSVG(const QString& bus_name);

    void GetRelevantBuses(const QStringView name = QStringView{},
                        const QStringView stops = QStringView{},
                        const std::optional<bool> is_roundtrip = std::nullopt,
                        const size_t color_index = -1,
                        const std::set<BusType>& bus_types = {},
                        const uint8_t capacity = 0,
                        const std::optional<bool> = false,
                        const std::optional<bool> = false,
                        const std::optional<bool> = false,
                        const std::optional<bool> = std::nullopt,
                        const uint8_t price = 0);

    void DrawBus(Bus* bus, QVBoxLayout* layout);

    void DrawStop(Stop* stop, QVBoxLayout* layout); 

    //void transferCatalogueDataToDatabase(DatabaseManager& db);

    template <typename Documents, typename Term>
    std::vector<Stop*> ComputeTfIdfs(const Documents& documents_, const Term& term) {
        std::map<Stop*, double> tf_idfs;
        int term_occurrences = 0;

        // Проходим по документам и считаем TF (Term Frequency)
        for (const auto& [key, document] : documents_) {
            auto words = SplitIntoWords(document->name);

            int count_term_in_document = std::count(words.begin(), words.end(), term);
             
            Stop* stop = const_cast<Stop*>(transport_catalogue_.FindStop(key));
            if (stop != nullptr) {
                tf_idfs[stop] = static_cast<double>(count_term_in_document) / words.size();

                if (count_term_in_document > 0) {
                    ++term_occurrences;
                }
            }
        }

        double idf = std::log(static_cast<double>(documents_.size()) / term_occurrences);

        for (auto& [stop, tf] : tf_idfs) {
            tf *= idf;
        }

        std::vector<Stop*> stops;
        for (const auto& [stop, tf] : tf_idfs) {
            if (tf != 0) {
                stops.push_back(stop);
            }
        }
         
        std::sort(stops.begin(), stops.end(), [&tf_idfs](Stop* lhs, Stop* rhs) {
            return tf_idfs.at(lhs) > tf_idfs.at(rhs);  
            });

        return stops;
    } 
};

#endif // MAINWINDOW_H
