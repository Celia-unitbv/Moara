#include "StartWindow.h"
#include "Styles.h"

#include <QPainter>
#include <QPushButton>

StartWindow::StartWindow(QWidget *parent)
	: QWidget(parent)
	, m_vsComputer(false)
	, m_bw(nullptr)
{
	ui.setupUi(this);
	
	connect(ui.BotButton, &QRadioButton::clicked, this, &StartWindow::onBotButtonClicked);

	QPushButton* startButton = new QPushButton("Start", this);
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

	setMinimumSize(400, 300);
	setMaximumSize(400, 300);
}

StartWindow::~StartWindow()
{
}

void StartWindow::paintEvent(QPaintEvent * event)
{
	QPainter p(this);
	QColor backgroundColor(238, 232, 170);

	p.fillRect(rect(), backgroundColor);
}

void StartWindow::onBotButtonClicked()
{
	m_vsComputer = ui.BotButton->isChecked();
}

void StartWindow::onStartButtonClicked()
{
	auto game = IGame::Produce(m_vsComputer);
	game->SetStrategy(EGameType::Easy);
	
	m_bw = new BoardWidget();
	m_bw->SetGame(game);

	game->AddListener(m_bw);
	game->Start();

	m_bw->show();
}
