#pragma once
#include"IGame.h"
#include "BoardWidget.h"
#include <QWidget>
#include "ui_StartWindow.h"

class StartWindow : public QWidget
{
	Q_OBJECT

public:
	StartWindow(QWidget *parent = nullptr);
	~StartWindow();

protected:
	void paintEvent(QPaintEvent* event);

private slots:
	void onStartButtonClicked();
	void onBotButtonClicked();

private:
	Ui::StartWindowClass ui;

	bool m_vsComputer;
	BoardWidget* m_bw;	
};
