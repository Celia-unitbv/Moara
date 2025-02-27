#include "GameWindow.h"
#include <QPushButton>
#include <QPalette>
#include <QPainter>
#include <QApplication>
#include <QScreen>

GameWindow::GameWindow(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	mainLayout = new QHBoxLayout(this);

	// Creează primul layout vertical și adaugă butoanele
	verticalLayout1 = new QVBoxLayout();

	verticalLayout1->setContentsMargins(20, 0, 20, 0);
	QPushButton* saveButton = new QPushButton("Save", this);
	QPushButton* loadButton = new QPushButton("Load", this);
	QPushButton* resetButton = new QPushButton("Reset", this);
	QPushButton* undoButton = new QPushButton("Undo", this);


	verticalLayout1->addWidget(saveButton);
	verticalLayout1->addWidget(loadButton);
	verticalLayout1->addWidget(resetButton);
	verticalLayout1->addWidget(undoButton);

	mainLayout->addLayout(verticalLayout1);

	verticalLayout2 = new QVBoxLayout();
	QPushButton* button2 = new QPushButton("Button 2", this);
	verticalLayout2->addWidget(button2);
	mainLayout->addLayout(verticalLayout2);

	verticalLayout3 = new QVBoxLayout();
	QPushButton* button3 = new QPushButton("Button 3", this);
	verticalLayout3->addWidget(button3);
	mainLayout->addLayout(verticalLayout3);

	setLayout(mainLayout);

	connect(saveButton, &QPushButton::clicked, this, &GameWindow::OnSaveButtonClicked);
	connect(loadButton, &QPushButton::clicked, this, &GameWindow::OnLoadButtonClicked);
	connect(resetButton, &QPushButton::clicked, this, &GameWindow::OnResetButtonClicked);
	connect(undoButton, &QPushButton::clicked, this, &GameWindow::OnUndoButtonClicked);

	QObject::connect(&m_msgBoxEndGame, &QMessageBox::buttonClicked, [&](QAbstractButton* button) {
		if (m_msgBoxEndGame.buttonRole(button) == QMessageBox::AcceptRole) {
			OnOkButtonClicked();
		}
		});
	
}



void GameWindow::OnSaveButtonClicked()
{
	m_msgBox.setText("Game saved successfully\n");
	m_msgBox.exec();
}

void GameWindow::OnLoadButtonClicked()
{
	
}

void GameWindow::OnResetButtonClicked()
{
	
}

void GameWindow::OnUndoButtonClicked()
{
	
}

void GameWindow::OnOkButtonClicked()
{
	this->close();
}

void GameWindow::paintEvent(QPaintEvent* event)
{
	QPainter p(this);
	QColor backgroundColor(238, 232, 170);

	p.fillRect(rect(), backgroundColor);
}

GameWindow::~GameWindow()
{}
