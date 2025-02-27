#include "BoardWidget.h"

#include "Styles.h"

#include <QPainter>
#include <QLabel>

BoardWidget::BoardWidget(bool vsComputer, const std::string& name, QWidget* parent)
	: QWidget(parent)
	, m_selectedNode1(nullptr)
	, m_selectedNode2(nullptr)
	, m_strategyOn(false)
	, m_vsComputer(vsComputer)
	, m_gameName(name)
	, m_client(nullptr)
	, m_state(EState::Place)
	, m_curentPlayer(0)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);

	SetupBoardWidget();
}

BoardWidget::~BoardWidget()
{
	m_client->EndGame();
}


void BoardWidget::OnResetButtonClicked()
{
	m_client->Reset();
}

void BoardWidget::OnUndoButtonClicked()
{
	m_client->Undo();
}


void BoardWidget::OnOkButtonClicked()
{
	close();
}



Pos BoardWidget::FindIndexFromMatrix(int val)
{
	for (int i = 0; i < m_nodeCoordinates.size(); i++)
	{
		for (int j = 0; j < m_nodeCoordinates[i].size(); j++)
		{
			if (m_nodeCoordinates[i][j] != -1 && m_nodeCoordinates[i][j] == val)
			{
				return std::make_pair(i, j);
			}
		}
	}
	return std::make_pair(-1, -1);
}

QString BoardWidget::GameExceptionToString(int exception)
{
	switch (exception)
	{
	case 1:
		return QString("The piece is part of a line.");
	case 2:
		return QString("This is not a valid place.");
	case 3:
		return QString("Index out of bounds.");
	case 4:
		return QString("This is not a valid piece.");
	default:
		return "";
	}
}


Node* BoardWidget::FindNodeFromScreenCoordinates(int x, int y)
{
	double minDistance = std::numeric_limits<double>::max();
	Node* selected = nullptr;

	for (auto node : m_nodes)
	{
		double distance = sqrt(pow(node->GetX() - x, 2) + pow(node->GetY() - y, 2));
		if (distance < minDistance)
		{
			minDistance = distance;
			selected = node;
		}
	}

	if (minDistance < 20)
		return selected;
	else
		return nullptr;
}

void BoardWidget::ShowHints(Node* nod)
{
	if (nod->GetType() != -1)
	{
		m_client->ShowHints(nod->GetInitialPoz());
	}
}

void BoardWidget::TimeAction()
{
	m_client->GetTime();

	m_timeAction >= (Miliseconds(7 * 1000)) ? m_timeActionLabel->setStyleSheet(labelStyleRed) : m_timeActionLabel->setStyleSheet(labelStyle);
	m_timeActionLabel->setText(FormatTimeAction(m_timeAction));
	m_timePlayer1Label->setText(FormatTime(m_timeActionPlayer1));
	m_timePlayer2Label->setText(FormatTime(m_timeActionPlayer2));

	QString msg = "State: ";

	switch (m_state)
	{
	case EState::Place:
		msg += "Place";
		break;
	case EState::Capture:
		msg += "Capture";
		break;
	case EState::Move:
		msg += "Move";
		break;
	};

	m_currentState->setText(msg);
}

Node* BoardWidget::FindNodeFromInitialPoz(Pos position)
{
	for (auto node : m_nodes)
	{
		if (node->GetInitialPoz() == position)
			return node;
	}
	return nullptr;
}

void BoardWidget::OnStrategyComboBoxClicked(int index)
{
	switch (index)
	{
	case 0:
		m_client->SetStrategy(index);
		break;
	case 1:
		m_client->SetStrategy(index);
		break;
	default:
	break;
	}
}

void BoardWidget::paintEvent(QPaintEvent* event)
{
	m_client->GetGameState();
	QPainter p(this);

	QColor backgroundColor(238, 232, 170);

	p.fillRect(rect(), backgroundColor);
	p.setPen(QPen(Qt::black, 2));

	int windowWidth = this->width();
	int windowHeight = this->height();


	float boardFraction = 0.15;
	int boardWidth = windowWidth * boardFraction;
	int boardHeight = windowHeight * boardFraction;

	int offsetX = (windowWidth - boardWidth) / 4;
	int offsetY = (windowHeight - boardHeight) / 4;

	float nodeSizeFraction = 0.17;
	float moveSizeFraction = 0.1;
	int nodeSize = boardWidth * nodeSizeFraction;
	int moveSize = boardWidth * moveSizeFraction;

	for (const auto& elem : m_nodes)
	{
		for (const auto& arc : elem->GetConnections())
		{
			p.drawLine(
				elem->GetX() * boardWidth / 100 + offsetX,
				elem->GetY() * boardHeight / 100 + offsetY,
				arc->GetX() * boardWidth / 100 + offsetX,
				arc->GetY() * boardHeight / 100 + offsetY
			);
		}
	}

	for (const auto& elem : m_nodes)
	{
		QColor color = backgroundColor;
		if (elem->GetType() == 0)
		{
			color = Qt::red;
		}
		else if (elem->GetType() == 1)
		{
			color = Qt::black;
		}

		p.setBrush(color);

		QRect r(
			elem->GetX() * boardWidth / 100 + offsetX - nodeSize / 2,
			elem->GetY() * boardHeight / 100 + offsetY - nodeSize / 2,
			nodeSize,
			nodeSize
		);
		p.drawEllipse(r);
	}

	if (m_hints.size() > 0)
	{
		p.setBrush(Qt::yellow);
		for (const auto& nod : m_hints)
		{
			Node* node = FindNodeFromInitialPoz(nod);
			QRect r(
				node->GetX() * boardWidth / 100 + offsetX - moveSize / 2,
				node->GetY() * boardHeight / 100 + offsetY - moveSize / 2,
				moveSize,
				moveSize
			);
			p.drawEllipse(r);
		}
	}

	if (!m_vsComputer)
	{
		QString currentPlayer = m_curentPlayer == 1 ? "Player 2" : "Player 1";
		m_playerToMove->setText(currentPlayer);
	}
	else
	{
		QString currentPlayer = m_curentPlayer == 1 ? "Robot" : "Human";
		m_playerToMove->setText(currentPlayer);
	}

	TimeAction();
}


QString BoardWidget::FormatTime(Miliseconds ms)
{
	auto secs = std::chrono::duration_cast<std::chrono::seconds>(ms);
	int minutes = static_cast<int>(secs.count()) / 60;
	int seconds = static_cast<int>(secs.count()) % 60;

	return QString("%1:%2")
		.arg(minutes, 2, 10, QChar('0'))
		.arg(seconds, 2, 10, QChar('0'));
}

QString BoardWidget::FormatTimeAction(Miliseconds ms)
{
	Miliseconds initialDuration(10 * 1000);
	auto secsElapsed = std::chrono::duration_cast<std::chrono::seconds>(ms);
	auto secsInitial = std::chrono::duration_cast<std::chrono::seconds>(initialDuration);
	auto secsRemaining = secsInitial - secsElapsed;

	int minutes = static_cast<int>(secsRemaining.count()) / 60;
	int seconds = static_cast<int>(secsRemaining.count()) % 60;

	return QString("%1:%2")
		.arg(minutes, 2, 10, QChar('0'))
		.arg(seconds, 2, 10, QChar('0'));
}

void BoardWidget::mousePressEvent(QMouseEvent* event)
{
	int mouseX = event->pos().x();
	int mouseY = event->pos().y();

	int windowWidth = this->width();
	int windowHeight = this->height();

	float boardFraction = 0.15;
	int boardWidth = windowWidth * boardFraction;
	int boardHeight = windowHeight * boardFraction;

	int offsetX = (windowWidth - boardWidth) / 4;
	int offsetY = (windowHeight - boardHeight) / 4;

	int relativeX = mouseX - offsetX;
	int relativeY = mouseY - offsetY;

	int boardX = relativeX * 100 / boardWidth;
	int boardY = relativeY * 100 / boardHeight;

	Node* clickedNode = FindNodeFromScreenCoordinates(boardX, boardY);

	
	
	if (m_curentPlayer == m_clientProxy)
	{
		if (m_state == EState::Move)
		{ 
			if ((!m_selectedNode1 && clickedNode))
			{
				if (clickedNode->GetType() == m_curentPlayer)
				{
					m_selectedNode1 = clickedNode;
					ShowHints(m_selectedNode1);
				}

				clickedNode = nullptr;
			}
		}

		if (clickedNode)
		{

			switch (m_state)
			{
			case EState::Place:
			{
				auto initialPoz = clickedNode->GetInitialPoz();
				m_client->PlacePiece(initialPoz);
				break;
			}
			case EState::Move:
			{
				if (m_selectedNode1)
				{

					auto initialPoz = m_selectedNode1->GetInitialPoz();
					auto initialPoz2 = clickedNode->GetInitialPoz();

					m_client->MovePiece(initialPoz, initialPoz2);

				}
			}
			break;
			case EState::Capture:
			{
				if (clickedNode->GetType() != m_curentPlayer)
				{
					auto initialPoz = clickedNode->GetInitialPoz();
					m_client->CapturePiece(initialPoz);
				}
				break;
			}
			}

		}
	}
	
	
	
	update();

}

void BoardWidget::SetClient(IClientPtr client)
{
	m_client = client;

	m_client->AddListener(this);

	m_client->StartGame();

	m_client->GetClientPlayer();
}

void BoardWidget::OnServerDisconnected()
{
	QMetaObject::invokeMethod(this, [this]() {
		m_msgBoxEndGame.setText("Connection lost. The game will now close.");
		m_msgBoxEndGame.exec();
		}, Qt::QueuedConnection);
}

void BoardWidget::OnOpponentLeft()
{
	QString msg = "Opponent left the game. You win!";
	QMetaObject::invokeMethod(this, [msg, this]() {
		m_msgBoxEndGame.setText(msg);
		m_msgBoxEndGame.exec();
		}, Qt::QueuedConnection);

}

void BoardWidget::OnGetTime(const std::vector<int>& data)
{
	m_timeActionPlayer1 = (Miliseconds)data[0];
	m_timeActionPlayer2 = (Miliseconds)data[1];
	m_timeAction = (Miliseconds)data[2];
}


void BoardWidget::OnReset()
{

	for (auto node : m_nodes)
	{
		node->SetType(-1);
	}
	m_hints.clear();

	m_selectedNode1 = nullptr;
	m_selectedNode2 = nullptr;

	update();
}


void BoardWidget::OnWin(int state)
{
	auto msg = state == 0 ? "Player 1 has won!!!!" : "Player 2 has won!!!!";

	QMetaObject::invokeMethod(this, [msg, this]()
		{
			m_msgBoxEndGame.setText(msg);
			m_msgBoxEndGame.exec();
		}, Qt::QueuedConnection);
}

void BoardWidget::OnError(int error)
{
	if (m_clientProxy == m_curentPlayer)
	{
		QString msg = GameExceptionToString(error);
		QMetaObject::invokeMethod(this, [msg, this]()
			{
				m_msgBox.setText(msg);
				m_msgBox.exec();
			}, Qt::QueuedConnection);
	}
}

void BoardWidget::OnPlacePiece(Pos placePos, int color)
{
	FindNodeFromInitialPoz(placePos)->SetType(color);
	m_client->GetTime();
	m_hints.clear();

	update();
}

void BoardWidget::OnMovePiece(Pos initialPos, Pos finalPos, int currentPlayer)
{
	FindNodeFromInitialPoz(initialPos)->SetType(-1);
	FindNodeFromInitialPoz(finalPos)->SetType(currentPlayer);
	m_client->GetTime();
	m_selectedNode1 = nullptr;
	m_hints.clear();
	update();

}
void BoardWidget::OnRemovePiece(Pos removePos)
{
	FindNodeFromInitialPoz(removePos)->SetType(-1);
	m_client->GetTime();

	m_hints.clear();
	update();
}

void BoardWidget::OnUpdateGameVariables(int state, int currentPlayer)
{
	m_state = (EState)state;
	m_curentPlayer = currentPlayer;
}

void BoardWidget::OnGetClientPlayer(int player)
{
	QString label = player == 0 ? "You are Player 1" : "You are Player 2";
	m_playerNetwork->setText(label);
	m_clientProxy = player;
}

void BoardWidget::OnUndo(const std::vector<int>& position,int state,int player)
{
	switch ((EState)state)
	{
	case EState::Place:
	{
		m_selectedNode1 = nullptr;

		for (auto node : m_nodes)
		{
			if (node->GetInitialPoz().first == position[0] && node->GetInitialPoz().second == position[1])
			{
				node->SetType(-1);
				break;
			}
		}
		break;
	}
	case EState::Move:
	{
		m_selectedNode1 = nullptr;
		if (position.size() == 7)
			for (auto node : m_nodes)
			{
				if (node->GetInitialPoz().first == position[2] && node->GetInitialPoz().second == position[3])
				{
					node->SetType(-1);
					node->GetMoves().clear();
				}
				if (node->GetInitialPoz().first == position[0] && node->GetInitialPoz().second == position[1])
				{
					node->SetType(player);
				}
			}
	}
	break;
	case EState::Capture:
	{

		for (auto node : m_nodes)
		{
			if (node->GetInitialPoz().first == position[0] && node->GetInitialPoz().second == position[1])
			{
				node->SetType(1 - player);
			}
		}
		break;
	}
	}

	update();
}


void BoardWidget::OnShowHints(const std::vector<int>& data)
{
	m_hints.clear();
	if (data.size() == 0)
	{
		QMetaObject::invokeMethod(this, [this]() {
			m_msgBox.setText("There is no possible action for this piece");
			m_msgBox.exec();
			}, Qt::QueuedConnection);
		return;
	}
	for (int i = 0; i < data.size()-1; i+=2)
	{
		m_hints.push_back({ data[i],data[i + 1] });
	}
}

void BoardWidget::InitializeLayouts()
{
	m_playerLayout = new QHBoxLayout();

	m_playerToMove = new QLabel("Player 1", this);
	m_playerNetwork = new QLabel();
	m_currentState = new QLabel();

	QFont font;
	font.setPointSize(16);
	font.setBold(true);

	m_playerNetwork->setFont(font);
	m_currentState->setFont(font);

	QVBoxLayout* m_topLayout = new QVBoxLayout();
	m_topLayout->setSpacing(2);
	m_topLayout->setContentsMargins(0, 0, 0, 0);

	m_topLayout->addWidget(m_playerNetwork, 0, Qt::AlignTop | Qt::AlignHCenter);
	m_topLayout->addWidget(m_currentState, 0, Qt::AlignTop | Qt::AlignHCenter);

	QSpacerItem* leftSpacer = new QSpacerItem(30, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);
	QSpacerItem* topSpacer = new QSpacerItem(0, 50, QSizePolicy::Minimum, QSizePolicy::Fixed);

	m_playerLayout->addSpacerItem(leftSpacer);
	m_playerLayout->addSpacerItem(topSpacer);

	m_playerToMove->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
	m_playerLayout->addWidget(m_playerToMove, 0, Qt::AlignTop | Qt::AlignHCenter);
	m_playerToMove->setStyleSheet(labelPlayerStyle);


	strategyComboBox = new QComboBox(this);
	strategyComboBox->addItem("Easy");
	strategyComboBox->addItem("Medium");
	strategyComboBox->setStyleSheet(ComboBoxStyle);

	resetButton = new QPushButton("Reset", this);
	undoButton = new QPushButton("Undo", this);

	resetButton->setStyleSheet(buttonStyle);
	undoButton->setStyleSheet(buttonStyle);

	QSpacerItem* verticalSpacer = new QSpacerItem(20, 100, QSizePolicy::Minimum, QSizePolicy::Expanding);

	m_layout1 = new QVBoxLayout();
	m_layout1->setContentsMargins(10, 10, 10, 50);
	m_layout1->addLayout(m_topLayout, 0);
	m_layout1->addSpacerItem(verticalSpacer); 
	m_layout1->addWidget(resetButton);
	m_layout1->addWidget(undoButton);
	m_layout1->addWidget(strategyComboBox); 
	m_layout1->addStretch(); 

	QLabel* time1Label = new QLabel("Time for player 1: ", this);
	m_timePlayer1Label = new QLabel("01:00", this);
	QLabel* time2Label = new QLabel("Time for player 2:", this);
	m_timePlayer2Label = new QLabel("01:00", this);
	QLabel* time3Label = new QLabel("Time for action:", this);
	m_timeActionLabel = new QLabel("00:10", this);

	m_timeActionLabel->setStyleSheet(labelStyle);
	time1Label->setStyleSheet(labelStyle);
	time2Label->setStyleSheet(labelStyle);
	time3Label->setStyleSheet(labelStyle);
	m_timePlayer1Label->setStyleSheet(labelStyle);
	m_timePlayer2Label->setStyleSheet(labelStyle);

	QHBoxLayout* player1Layout = new QHBoxLayout();
	player1Layout->addWidget(time1Label);
	player1Layout->addWidget(m_timePlayer1Label);

	QHBoxLayout* player2Layout = new QHBoxLayout();
	player2Layout->addWidget(time2Label);
	player2Layout->addWidget(m_timePlayer2Label);

	QHBoxLayout* actionLayout = new QHBoxLayout();
	actionLayout->addWidget(time3Label);
	actionLayout->addWidget(m_timeActionLabel);

	m_layout2 = new QVBoxLayout();
	m_layout2->addLayout(player1Layout);
	m_layout2->addLayout(player2Layout);
	m_layout2->addLayout(actionLayout);

	m_hLayout = new QHBoxLayout();
	m_hLayout->addLayout(m_layout1);
	m_hLayout->addStretch();
	m_hLayout->addLayout(m_playerLayout);
	m_hLayout->addStretch();
	m_hLayout->addLayout(m_layout2);
}


void BoardWidget::InitializeWidgets()
{
	setLayout(m_hLayout);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setMinimumSize(900, 500);
}

void BoardWidget::ConnectSignalsAndSlots()
{

	connect(resetButton, &QPushButton::clicked, this, &BoardWidget::OnResetButtonClicked);
	connect(undoButton, &QPushButton::clicked, this, &BoardWidget::OnUndoButtonClicked);
	connect(strategyComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &BoardWidget::OnStrategyComboBoxClicked);
	QObject::connect(&m_msgBoxEndGame, &QMessageBox::buttonClicked, [&](QAbstractButton* button) {
		if (m_msgBoxEndGame.buttonRole(button) == QMessageBox::AcceptRole) {
			OnOkButtonClicked();
		}
		});

	/*connect(saveButton, &QPushButton::clicked, this, &BoardWidget::OnSaveButtonClicked);
	
	connect(resetButton, &QPushButton::clicked, this, &BoardWidget::OnResetButtonClicked);
	
	connect(strategyComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &BoardWidget::OnStrategyComboBoxClicked);

	QObject::connect(&m_msgBoxEndGame, &QMessageBox::buttonClicked, [&](QAbstractButton* button) {
		if (m_msgBoxEndGame.buttonRole(button) == QMessageBox::AcceptRole) {
			OnOkButtonClicked();
		}
		});*/

	strategyComboBox->setVisible(m_vsComputer);
}

void BoardWidget::InitializeBoard()
{
	m_nodeCoordinates = {
		{0,-1,-1,7,-1,-1,6},
		{-1,8,-1,15,-1,14,-1},
		{-1,-1,16,23,22,-1,-1},
		{1,9,17,-1,21,13,5},
		{-1,-1,18,19,20,-1,-1},
		{-1,10,-1,11,-1,12,-1},
		{2,-1,-1,3,-1,-1,4}
	};

	m_gameBoard = {
		{0,1,2,3,4,5,6,7},
		{0,1,2,3,4,5,6,7},
		{0,1,2,3,4,5,6,7}
	};

	for (int i = 0; i < m_gameBoard.size(); i++) {
		for (int j = 0; j < m_gameBoard[i].size(); j++) {
			Node* n = new Node(i, j);
			m_nodes.push_back(n);
		}
	}

	for (int i = 0; i < m_gameBoard.size(); i++)
	{
		for (int j = 0; j < m_gameBoard[i].size(); j++)
		{
			std::vector<Node*> aux;
			if (m_gameBoard[i][j] % 2 == 0)
			{
				aux.push_back(m_nodes[i * 8 + (j - 1 + 8) % 8]);
				aux.push_back(m_nodes[i * 8 + j + 1]);
			}
			else
			{
				aux.push_back(m_nodes[i * 8 + j - 1]);
				aux.push_back(m_nodes[i * 8 + (j + 1) % 8]);
				if (i == 0)
					aux.push_back(m_nodes[(i + 1) * 8 + j]);
				else if (i == 2)
					aux.push_back(m_nodes[(i - 1) * 8 + j]);
				else
				{
					aux.push_back(m_nodes[(i + 1) * 8 + j]);
					aux.push_back(m_nodes[(i - 1) * 8 + j]);
				}
			}
			m_nodes[i * 8 + j]->SetConnections(aux);
		}
	}

	for (int i = 0; i < m_nodes.size(); i++)
	{
		Pos cor = FindIndexFromMatrix(i);
		m_nodes[i]->SetXY(cor);
	}
}

void BoardWidget::SetupBoardWidget()
{
	InitializeLayouts();
	InitializeWidgets();
	ConnectSignalsAndSlots();
	InitializeBoard();
}