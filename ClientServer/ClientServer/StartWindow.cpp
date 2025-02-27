#include "StartWindow.h"
#include "Styles.h"

#include <QPainter>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <iostream>
#include <functional>

StartWindow::StartWindow(QWidget* parent)
	: QWidget(parent)
	, m_vsComputer(false)
	, m_bw(nullptr)
{
	ui.setupUi(this);

	m_client = IClient::Produce();

	Style();
}


StartWindow::~StartWindow()
{
	
}

void StartWindow::OnStart()
{
	QMetaObject::invokeMethod(this, [this]()
		{
			m_client->RemoveListener(this);
			m_bw = new BoardWidget(m_vsComputer, lineEdit->text().toStdString());
			m_bw->SetClient(m_client);
			m_bw->show();

		}, Qt::QueuedConnection);
}

void StartWindow::OnGameAlreadyExists()
{
	QMetaObject::invokeMethod(this, [this]()
		{
			QMessageBox msg;
			msg.setText("Choose another game name!");
			msg.exec();
		}, Qt::QueuedConnection);
}

void StartWindow::paintEvent(QPaintEvent* event)
{
	QPainter p(this);
	QColor backgroundColor(238, 232, 170);

	p.fillRect(rect(), backgroundColor);
}

void StartWindow::Style()
{
	connect(ui.BotButton, &QRadioButton::clicked, this, &StartWindow::onBotButtonClicked);

	startButton = new QPushButton("Start", this);
	connect(startButton, &QPushButton::clicked, this, &StartWindow::onStartButtonClicked);
	startButton->setStyleSheet(buttonStyle);

	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addStretch();
	vLayout->addStretch();
	vLayout->addWidget(startButton);
	vLayout->addStretch();

	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->addStretch();
	hLayout->addLayout(vLayout);
	hLayout->addStretch();
	setLayout(hLayout);

	lineEdit = new QLineEdit;
	lineEdit->setText("gameName");
	ui.hostLineEdit->setText("192.168.2.30");
	ui.portLineEdit->setText("53000");

	vLayout->addWidget(lineEdit);

	setMinimumSize(400, 300);
	setMaximumSize(400, 300);
}

void StartWindow::onBotButtonClicked()
{
	m_vsComputer = ui.BotButton->isChecked();
}

void StartWindow::onStartButtonClicked()
{
	std::string gameName = lineEdit->text().toStdString();
	std::string host = ui.hostLineEdit->text().toStdString();
	int port = ui.portLineEdit->text().toInt();
	m_client->SetConnectionInfo(host, port);

	m_client->AddListener(this);

	if (!m_client->Start( m_vsComputer, gameName))
	{
		QMessageBox msg;
		msg.setText("wait for a few seconds and start again.");
		msg.exec();
	}
}
