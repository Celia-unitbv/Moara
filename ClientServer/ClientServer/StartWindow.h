#pragma once

#include "BoardWidget.h"


#include "IClient.h"

#include "ui_StartWindow.h"

#include <QWidget>
#include <QPushButton>

class StartWindow : public QWidget,public IClientListener
{
	Q_OBJECT

public:
	StartWindow(QWidget* parent = nullptr);
	~StartWindow();

	void OnStart() override;

	void OnGameAlreadyExists() override;

protected:
	void paintEvent(QPaintEvent* event);
	void Style();

private slots:
	void onStartButtonClicked();
	void onBotButtonClicked();

private:
	Ui::StartWindowClass ui;
	BoardWidget* m_bw;

	QLineEdit* lineEdit;
	QPushButton* startButton;
	bool m_vsComputer;
	
	IClientPtr m_client;
};
