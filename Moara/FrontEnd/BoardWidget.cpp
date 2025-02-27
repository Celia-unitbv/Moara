#include "BoardWidget.h"
#include "Styles.h"

#include <QPainter>
#include <qtimer.h>

BoardWidget::BoardWidget(QWidget* parent)
	: QWidget(parent)
	, m_selectedNode1(nullptr)
	, m_selectedNode2(nullptr)
	, m_strategyOn(false)
{
	ui.setupUi(this);
	
	setupBoardWidget();
	
}

BoardWidget::~BoardWidget()
{
}

void BoardWidget::onOkButtonClicked()
{
	close();
}

void BoardWidget::OnSaveButtonClicked()
{
	auto msg = m_game->SaveGame("interfaceSave", "SaveHistory") ? "Game saved successfully\n" : "Failed to save the game\n";
	m_msgBox.setText(msg);
	
	m_msgBox.exec();
}

void BoardWidget::OnPlacePiece(Pos placePos, int color)
{
	FindNodeFromInitialPoz(placePos)->SetType(color);

	update();
}

void BoardWidget::OnRemovePiece(Pos removePos)
{
	FindNodeFromInitialPoz(removePos)->SetType(-1);

	update();
}

void BoardWidget::OnLoad()
{
	OnReset();

	m_player->setText(m_game->GetCurrentPlayer() == EPlayer::Player1 ? "Player 1" : "Player 2");

	for (auto row : m_game->GetBoard())
	{
		for (auto elem : row)
		{
			if (elem)
			{
				Node* aux = FindNodeFromInitialPoz(elem->GetPozition());
				aux->SetType((int)elem->GetColor());
			}
		}
	}

	update();
}

void BoardWidget::OnReset()
{
	for (auto node : m_nodes)
	{
		node->SetType(-1);
		node->GetMoves().clear();
	}

	m_selectedNode1 = nullptr;
	m_selectedNode2 = nullptr;

	m_player->setText(m_game->GetCurrentPlayer() == EPlayer::Player1 ? "Player 1" : "Player 2");

	m_timeActionPlayer1 = m_game->GetTime(EPlayer::Player1);
	m_timeActionPlayer2 = m_game->GetTime(EPlayer::Player2);

	m_timePlayer1Label->setText(FormatTime(m_timeActionPlayer1));
	m_timePlayer2Label->setText(FormatTime(m_timeActionPlayer2));
	
	update();
}

void BoardWidget::OnSave()
{
	
}

void BoardWidget::OnUndo(const Positions& position)
{
	m_selectedNode2 = nullptr;

	switch (m_game->GetGameState())
	{
	case EState::Place:
	{
		m_selectedNode1 = nullptr;
		for (auto node : m_nodes)
		{
			if (node->GetInitialPoz() == position[0])
			{
				node->SetType(-1);
				break;
			}
		}

		break;
	}
	case EState::Capture:
	{
		for (auto node : m_nodes)
		{
			if (node->GetInitialPoz() == position[0])
			{
				node->SetType(1 - (int)m_game->GetCurrentPlayer());
			}
			else if (node->GetType() == (int)m_game->GetCurrentPlayer())
			{
				m_selectedNode1 = node;
			}
		}

		m_selectedNode1->SetMoves(WhereToMove(m_selectedNode1));

		break;
	}
	case EState::Move:
	{
		m_selectedNode1 = nullptr;

		for (auto node : m_nodes)
		{
			if (node->GetInitialPoz() == position[1])
			{
				node->SetType(-1);
				node->GetMoves().clear();
			}
			if (node->GetInitialPoz() == position[0])
			{
				node->SetType((int)m_game->GetCurrentPlayer());
			}
		}
	}
	break;
	}

	m_msgBox.setText(QString("Game State: " + StateToString(m_game->GetGameState())));
	m_msgBox.exec();

	m_player->setText(m_game->GetCurrentPlayer() == EPlayer::Player1 ? "Player 1" : "Player 2");

	update();
}

void BoardWidget::OnMovePiece(Pos initialPos, Pos finalPos, int m_currentPlayer)
{
	FindNodeFromInitialPoz(initialPos)->SetType(-1);
	FindNodeFromInitialPoz(finalPos)->SetType(m_currentPlayer);

	update();
}

void BoardWidget::OnWin()
{
	auto msg = m_game->GetGameState() == EState::Player2Won ? "Player 2 has won!!!!" : "Player 1 has won!!!!";
	m_msgBoxEndGame.setText(msg);

	m_msgBoxEndGame.exec();
}

void BoardWidget::SetGame(IGamePtr game)
{
	this->m_game = game;

	m_player->setText(m_game->GetCurrentPlayer() == EPlayer::Player1 ? "Player 1" : "Player 2");

	m_timeActionPlayer1 = m_game->GetTime(EPlayer::Player1);
	m_timeActionPlayer2 = m_game->GetTime(EPlayer::Player2);

	m_timePlayer1Label->setText(FormatTime(m_timeActionPlayer1));
	m_timePlayer2Label->setText(FormatTime(m_timeActionPlayer2));

	strategyComboBox->setVisible(m_game->GetGameVsComputer());
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

QString BoardWidget::GameExceptionToString(EOperationResult exception)
{
	switch (exception)
	{
	case EOperationResult::PieceIsPartOfALine:
		return  QString("The piece is part of a line.");
	case EOperationResult::IsNotAValidPlace:
		return QString("This is not a valid place.");
	case EOperationResult::IndexOutOfBounds:
		return QString("Index out of bounds.");
	case EOperationResult::IsNotAValidPiece:
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

std::vector<Node*> BoardWidget::WhereToMove(Node* nod)
{
	Positions moves;
	if (nod->GetType() != -1)
	{
		if (m_game->GetGameState() == EState::Move)
			moves = m_game->WhereToMove(nod->GetInitialPoz());
		else if (m_game->GetGameState() == EState::Capture)
			moves = m_game->WhereToCapture();
	}
			
	std::vector<Node*>finalMoves;
	for (auto it : moves)
	{
		finalMoves.emplace_back(FindNodeFromInitialPoz(it));
	}
	return finalMoves;
}

void BoardWidget::TimeAction(EPlayer player)
{
	Miliseconds time = m_game->GetTimeAction();
	 time >= (Miliseconds(7 * 1000)) ? m_timeActionLabel->setStyleSheet(labelStyleRed) : m_timeActionLabel->setStyleSheet(labelStyle);
	m_timeActionLabel->setText(FormatTimeAction(time));
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

void BoardWidget::OnLoadButtonClicked()
{
	if (!m_game->LoadGame("interfaceSave"))
	{
		m_msgBox.setText("Failed to load a game!");
		m_msgBox.exec();
	}
}

void BoardWidget::OnResetButtonClicked()
{
	m_game->ResetGame();
}

void BoardWidget::OnUndoButtonClicked()
{

	m_game->Undo();
}

void BoardWidget::OnStrategyComboBoxClicked(int index)
{
	if (m_game)
		switch (index)
		{
		case 0:
			m_game->SetStrategy(EGameType::Easy);
			break;
		case 1:
			m_game->SetStrategy(EGameType::Medium);
			break;
		default:
		{
			m_msgBox.setText("No option selected! The game runs on easy strategy!");
			m_msgBox.exec();
		}
		break;
		}
}


void BoardWidget::paintEvent(QPaintEvent* event)
{
	QPainter p(this);

	QColor backgroundColor(238, 232, 170);
	p.fillRect(rect(), backgroundColor);

	p.setPen(QPen(Qt::black, 2));

	// Calculate the dimensions of the widget
	int windowWidth = this->width();
	int windowHeight = this->height();

	// Define the size of the board as a fraction of the widget's dimensions
	float boardFraction = 0.15; // Fraction of the window size for the board
	int boardWidth = windowWidth * boardFraction;
	int boardHeight = windowHeight * boardFraction;

	// Calculate offsets to center the board
	int offsetX = (windowWidth - boardWidth) / 4;
	int offsetY = (windowHeight - boardHeight) / 4;

	// Scale factors for elements (increase these values for larger circles)
	float nodeSizeFraction = 0.17; // Fraction of board size for node size (larger circles)
	float moveSizeFraction = 0.1; // Fraction of board size for move size (larger circles)
	int nodeSize = boardWidth * nodeSizeFraction;
	int moveSize = boardWidth * moveSizeFraction;

	// Draw connections
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

	// Draw nodes
	for (const auto& elem : m_nodes)
	{
		QColor color = backgroundColor;
		if (elem->GetType() == 0)
		{
			color = Qt::red;
		}
		else if(elem->GetType() == 1)
		{
			color = Qt::black;
		}

		p.setBrush(color);

		QRect r(
			elem->GetX() * boardWidth / 100 + offsetX - nodeSize / 2 ,
			elem->GetY() * boardHeight / 100 + offsetY - nodeSize / 2,
			nodeSize,
			nodeSize
		);
		p.drawEllipse(r);
	}

	// Highlight selected node's possible moves
	if (m_selectedNode1)
	{
		p.setBrush(Qt::yellow);
		for (const auto& node : m_selectedNode1->GetMoves())
		{
			QRect r(
				node->GetX() * boardWidth / 100 + offsetX - moveSize / 2,
				node->GetY() * boardHeight / 100 + offsetY - moveSize / 2,
				moveSize,
				moveSize
			);
			p.drawEllipse(r);
		}
	}

	TimeAction(m_game->GetCurrentPlayer());

	m_player->setText(m_game->GetCurrentPlayer() == EPlayer::Player1 ? "Player 1" : "Player 2");

	m_timePlayer1Label->setText(FormatTime(m_game->GetTime(EPlayer::Player1)));
	m_timePlayer2Label->setText(FormatTime(m_game->GetTime(EPlayer::Player2)));
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

QString BoardWidget::FormatTimeAction(Miliseconds ms )
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

		// Calculate the dimensions of the widget
	int windowWidth = this->width();
	int windowHeight = this->height();

		// Define the size of the board as a fraction of the widget's dimensions
	float boardFraction = 0.15; // Fraction of the window size for the board
	int boardWidth = windowWidth * boardFraction;
	int boardHeight = windowHeight * boardFraction;

		// Calculate offsets to position the board correctly
	int offsetX = (windowWidth - boardWidth) / 4;
	int offsetY = (windowHeight - boardHeight) / 4;

		// Adjust mouse coordinates to match the board position
	int relativeX = mouseX - offsetX;
	int relativeY = mouseY - offsetY;

		// Convert adjusted coordinates to board coordinates
	int boardX = relativeX * 100 / boardWidth;
	int boardY = relativeY * 100 / boardHeight;

		// Find the node closest to the converted board coordinates
	Node* clickedNode = FindNodeFromScreenCoordinates(boardX, boardY);

		if (clickedNode)
		{
			if (m_game->GetGameVsComputer())
			{
				if (m_game->GetCurrentPlayer() == EPlayer::Player2)
					return;
				if (m_selectedNode1 == nullptr)
				{
					if (clickedNode->GetType() == -1 || clickedNode->GetType() == (int)m_game->GetCurrentPlayer())
						m_selectedNode1 = clickedNode;

				}
				else
				{
					m_selectedNode2 = clickedNode;
				}
			}
			else
			{
				if (m_selectedNode1 == nullptr)
				{
					if (clickedNode->GetType() == -1 || clickedNode->GetType() == (int)m_game->GetCurrentPlayer())
						m_selectedNode1 = clickedNode;

				}
				else
				{

					m_selectedNode2 = clickedNode;
				}
			}

			


			// Handle the selected nodes based on the game state
			switch (m_game->GetGameState())
			{
			case EState::Place:
			{
				if (m_selectedNode1)
				{
					EOperationResult result = m_game->PlacePiece(m_selectedNode1->GetInitialPoz());
					if (result != EOperationResult::NoError)
					{
						m_selectedNode1 = nullptr;
						m_msgBox.setText(GameExceptionToString(result));
						m_msgBox.exec();
					}
					else
					{
						if (m_game->GetGameState() == EState::Capture)
						{
							m_selectedNode1->GetMoves().clear();
							m_selectedNode1->SetMoves(WhereToMove(m_selectedNode1));
						}
						else
						{
							m_selectedNode1 = nullptr;
						}
					}
				}
				break;
			}
			case EState::Move:
			{
				if (m_selectedNode1 != nullptr)
				{
					m_selectedNode1->GetMoves().clear();
					m_selectedNode1->SetMoves(WhereToMove(m_selectedNode1));
					if (m_selectedNode1->GetMoves().size() == 0)
					{
						m_selectedNode1 = nullptr;
						m_msgBox.setText("No possible move. Choose another Piece!");
						m_msgBox.exec();
					}
					else
					{
						if (m_selectedNode2 && m_selectedNode2->GetType() != m_selectedNode1->GetType())
						{
							EOperationResult result = m_game->MovePiece(m_selectedNode1->GetInitialPoz(), m_selectedNode2->GetInitialPoz());
							if (result != EOperationResult::NoError)
							{
								m_selectedNode1 = nullptr;
								m_selectedNode2 = nullptr;
								m_msgBox.setText(GameExceptionToString(result));
								m_msgBox.exec();
							}
							else
							{
								if (m_game->GetGameState() == EState::Capture)
								{
									m_selectedNode1 = m_selectedNode2;
									m_selectedNode1->GetMoves().clear();
									m_selectedNode1->SetMoves(WhereToMove(m_selectedNode1));
									m_selectedNode2 = nullptr;
								}
								else
								{
									m_selectedNode1 = nullptr;
									m_selectedNode2 = nullptr;
								}
							}
						}
						else if (m_selectedNode2)
						{
							m_selectedNode1 = m_selectedNode2;
							m_selectedNode1->GetMoves().clear();
							m_selectedNode1->SetMoves(WhereToMove(m_selectedNode1));
							m_selectedNode2 = nullptr;
						}
					}
				}
				break;
			}
			case EState::Capture:
			{
				if (m_selectedNode2 && m_selectedNode2->GetType() != m_selectedNode1->GetType())
				{
					EOperationResult result = m_game->RemovePiece(m_selectedNode2->GetInitialPoz());
					if (result != EOperationResult::NoError)
					{
						m_selectedNode2 = nullptr;
						m_msgBox.setText(GameExceptionToString(result));
						m_msgBox.exec();
					}
					else
					{
						m_selectedNode1 = nullptr;
						m_selectedNode2 = nullptr;
					}
				}
				else if (m_selectedNode2)
				{
					m_selectedNode1 = m_selectedNode2;
					m_selectedNode1->GetMoves().clear();
					m_selectedNode1->SetMoves(WhereToMove(m_selectedNode1));
					m_selectedNode2 = nullptr;
				}
				break;
			}
			}
		}

		m_player->setText(m_game->GetCurrentPlayer() == EPlayer::Player1 ? "Player 1" : "Player 2");

		m_timeActionPlayer1 = m_game->GetTime(EPlayer::Player1);
		m_timeActionPlayer2 = m_game->GetTime(EPlayer::Player2);
	

}

QString BoardWidget::StateToString(EState state) {
	switch (state) {
	case EState::Player1Won:
		return "Player1Won";
	case EState::Player2Won:
		return "Player2Won";
	case EState::Place:
		return "Place";
	case EState::Capture:
		return "Capture";
	case EState::Move:
		return "Move";
	}
	return "";
}

void BoardWidget::initializeLayouts() 
{
	m_playerLayout = new QHBoxLayout();
	m_player = new QLabel("Player 1", this);
	m_player->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
	m_playerLayout->addWidget(m_player, 0, Qt::AlignTop | Qt::AlignHCenter);
	m_player->setStyleSheet(labelPlayerStyle);

	saveButton = new QPushButton("Save", this);
	loadButton = new QPushButton("Load", this);
	resetButton = new QPushButton("Reset", this);
	undoButton = new QPushButton("Undo", this);

	loadButton->setStyleSheet(buttonStyle);
	saveButton->setStyleSheet(buttonStyle);
	resetButton->setStyleSheet(buttonStyle);
	undoButton->setStyleSheet(buttonStyle);

	m_layout1 = new QVBoxLayout();
	m_layout1->setContentsMargins(20, 0, 20, 0);
	m_layout1->setSpacing(10);
	m_layout1->addWidget(saveButton);
	m_layout1->addWidget(loadButton);
	m_layout1->addWidget(resetButton);
	m_layout1->addWidget(undoButton);

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

void BoardWidget::initializeWidgets() 
{
	strategyComboBox = new QComboBox(this);
	strategyComboBox->addItem("Easy");
	strategyComboBox->addItem("Medium");
	m_layout1->addWidget(strategyComboBox);
	strategyComboBox->setStyleSheet(ComboBoxStyle);

	setLayout(m_hLayout);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setMinimumSize(600, 400);
}

void BoardWidget::connectSignalsAndSlots() 
{
	connect(saveButton, &QPushButton::clicked, this, &BoardWidget::OnSaveButtonClicked);
	connect(loadButton, &QPushButton::clicked, this, &BoardWidget::OnLoadButtonClicked);
	connect(resetButton, &QPushButton::clicked, this, &BoardWidget::OnResetButtonClicked);
	connect(undoButton, &QPushButton::clicked, this, &BoardWidget::OnUndoButtonClicked);
	connect(strategyComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &BoardWidget::OnStrategyComboBoxClicked);

	QObject::connect(&m_msgBoxEndGame, &QMessageBox::buttonClicked, [&](QAbstractButton* button) {
		if (m_msgBoxEndGame.buttonRole(button) == QMessageBox::AcceptRole) {
			onOkButtonClicked();
		}
		});
}

void BoardWidget::initializeBoard() 
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

	for (int i = 0; i < m_gameBoard.size(); i++) {
		for (int j = 0; j < m_gameBoard[i].size(); j++) {
			std::vector<Node*> aux;
			if (m_gameBoard[i][j] % 2 == 0) {
				aux.push_back(m_nodes[i * 8 + (j - 1 + 8) % 8]);
				aux.push_back(m_nodes[i * 8 + j + 1]);
			}
			else {
				aux.push_back(m_nodes[i * 8 + j - 1]);
				aux.push_back(m_nodes[i * 8 + (j + 1) % 8]);
				if (i == 0)
					aux.push_back(m_nodes[(i + 1) * 8 + j]);
				else if (i == 2)
					aux.push_back(m_nodes[(i - 1) * 8 + j]);
				else {
					aux.push_back(m_nodes[(i + 1) * 8 + j]);
					aux.push_back(m_nodes[(i - 1) * 8 + j]);
				}
			}
			m_nodes[i * 8 + j]->SetConnections(aux);
		}
	}

	for (int i = 0; i < m_nodes.size(); i++) {
		Pos cor = FindIndexFromMatrix(i);
		m_nodes[i]->SetXY(cor);
	}
}

void BoardWidget::setupBoardWidget() 
{
	initializeLayouts();
	initializeWidgets();
	connectSignalsAndSlots();
	initializeBoard();
}