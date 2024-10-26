#pragma once

#include <QMainWindow>
#include "ui_stop_editor.h"

class StopEditor : public QMainWindow
{
	Q_OBJECT

public:
	StopEditor(QWidget *parent = nullptr);
	~StopEditor();

private:
	Ui::StopEditorClass ui;
};
