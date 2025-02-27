#pragma once
#include "ui_GameWindow.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>


class GameWindow : public QWidget
{
	Q_OBJECT

public:
	GameWindow(QWidget *parent = nullptr);
	~GameWindow();

private slots:
	void OnSaveButtonClicked();
	void OnLoadButtonClicked();
	void OnResetButtonClicked();
	void OnUndoButtonClicked();
	void OnOkButtonClicked();

protected:
	void paintEvent(QPaintEvent* event);

private:
	Ui::GameWindowClass ui;

	QHBoxLayout* mainLayout;
	QVBoxLayout* verticalLayout1;
	QVBoxLayout* verticalLayout2;
	QVBoxLayout* verticalLayout3;
	QMessageBox m_msgBoxEndGame;
	QMessageBox m_msgBox;
};
