#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once

#include <QMainWindow>
#include <QDialog>
#include <QObject>

#include "stdafx.h"
#include "database_manager.h"
#include "bus_editor.h"
#include "stop_editor.h"
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
    void on_button_distances_clicked();

    void on_search_bus_clicked();
    void on_reset_all_filters_clicked();

    void on_stops_append_clicked();
    void on_stops_search_clicked();

    void on_search_distance_clicked();
    void on_add_distance_clicked();
    void on_delete_distance_clicked();

    void on_connect_to_db_clicked();
    void on_connect_to_db_default_clicked();

    void OpenEditBusDialog(Bus* bus);

    void on_lineEdit_find_stopname_textEdited(const QString &arg1);

    void on_stops_clear_clicked();

private:
    Ui::MainWindow *ui;

    DatabaseManager db_manager_;
    TransportCatalogue transport_catalogue_;

    void SetLineEditSettings();
    void SetLabelSettings();

    void DisplayMapOnLabel(const QString& bus_name);
    Value JsonToSVG(const QString& bus_name);

    void DrawRelevantBuses(
        const std::optional<QStringView> name = std::nullopt,
        const std::optional<QStringView> stops = std::nullopt,
        const std::optional<bool> is_roundtrip = std::nullopt,
        const std::optional<size_t> color_index = std::nullopt,
        const std::optional<std::set<BusType>>& bus_types = std::nullopt,
        const std::optional<uint8_t> capacity = std::nullopt,
        const std::optional<bool> = std::nullopt,
        const std::optional<bool> = std::nullopt,
        const std::optional<bool> = std::nullopt,
        const std::optional<bool> = std::nullopt,
        const std::optional<bool> = std::nullopt,
        const std::optional<uint8_t> price = std::nullopt,
        const std::optional<bool> sort_by_color_index = std::nullopt
    );

    void DrawBus(Bus* bus, QVBoxLayout* layout);

    void EditStop(Stop* stop);
    void InfoStop(Stop* stop);
    void DeleteStop(Stop* stop);
    void DrawStop(Stop* stop, QVBoxLayout* layout); 

    void LoadDistances();

    template <typename Documents>
    std::vector<Stop*> ComputeTfIdfs(const Documents& documents_, const QStringView term) {
        std::map<Stop*, double> tf_idfs;
        int term_occurrences = 0;
        QString termLower = term.toString().toLower();

        std::vector<Stop*> exact_match_stops;

        for (const auto& [key, document] : documents_) {
            QString nameLower = document->name.toLower();

            // Check for exact phrase match
            if (nameLower.contains(termLower)) {
                Stop* stop = const_cast<Stop*>(transport_catalogue_.FindStop(key));
                if (stop != nullptr) {
                    exact_match_stops.push_back(stop);
                    continue; // Skip TF-IDF calculation for exact match
                }
            }

            // Split into words for TF-IDF calculation
            auto words = SplitIntoWords(nameLower);

            int count_term_in_document = std::count(words.begin(), words.end(), termLower);
            Stop* stop = const_cast<Stop*>(transport_catalogue_.FindStop(key));
            if (stop != nullptr) {
                tf_idfs[stop] = static_cast<double>(count_term_in_document) / words.size();

                if (count_term_in_document > 0) {
                    ++term_occurrences;
                }
            }
        }

        if (term_occurrences == 0 && exact_match_stops.empty()) {
            return {};  // Return empty if no occurrences found
        }

        // Calculate IDF
        double idf = 0.0;
        if (term_occurrences > 0) {
            idf = std::log(static_cast<double>(documents_.size()) / term_occurrences);
        }

        for (auto& [stop, tf] : tf_idfs) {
            tf *= idf;
        }

        std::vector<Stop*> stops(exact_match_stops);
        for (const auto& [stop, tf] : tf_idfs) {
            if (tf != 0) {
                stops.push_back(stop);
            }
        }

        std::sort(stops.begin(), stops.end(), [&tf_idfs](Stop* lhs, Stop* rhs) {
            double lhsScore = tf_idfs.count(lhs) ? tf_idfs.at(lhs) : std::numeric_limits<double>::max();
            double rhsScore = tf_idfs.count(rhs) ? tf_idfs.at(rhs) : std::numeric_limits<double>::max();
            return lhsScore > rhsScore;
        });

        return stops;
    }
};

#endif // MAINWINDOW_H
