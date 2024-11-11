#ifndef STOP_EDITOR_H
#define STOP_EDITOR_H

#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <vector>

class StopEditor : public QDialog {
    Q_OBJECT

public:
    explicit StopEditor(QWidget* parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle("Добавить остановку");

        layout_ = new QVBoxLayout(this);
        layout_->setSizeConstraint(QLayout::SetFixedSize);  // Устанавливаем автоматическую подстройку размера

        // Кнопка "Добавить" всегда будет внизу
        buttonAdd_ = new QPushButton("Добавить", this);
        layout_->addWidget(buttonAdd_);

        connect(buttonAdd_, &QPushButton::clicked, this, &StopEditor::onAddClicked);
    }

    // Метод для добавления поля ввода с меткой
    QLineEdit* addField(const QString& labelText) {
        QLabel* label = new QLabel(labelText, this);
        QLineEdit* lineEdit = new QLineEdit(this);

        layout_->insertWidget(layout_->count() - 1, label);  // Вставляем перед кнопкой
        layout_->insertWidget(layout_->count() - 1, lineEdit);

        fields_.push_back(lineEdit);
        return lineEdit;
    }

    // Получаем текст из конкретного поля
    QString getFieldText(QLineEdit* field) const {
        return field->text();
    }

private slots:
    void onAddClicked() {
        for (QLineEdit* field : fields_) {
            if (field->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Пожалуйста, заполните все поля.");
                return;
            }
        }

        emit fieldsEntered();  // Сигнал о завершении ввода
        accept();
    }

signals:
    void fieldsEntered();  // Сигнал для передачи данных

private:
    QVBoxLayout* layout_;
    QPushButton* buttonAdd_;
    std::vector<QLineEdit*> fields_;
};

#endif // STOP_EDITOR_H
