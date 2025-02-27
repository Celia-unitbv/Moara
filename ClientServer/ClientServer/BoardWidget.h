#pragma once

#include "Node.h"
#include "IClient.h"

#include "ui_BoardWidget.h"

#include <QWidget>
#include <QMouseEvent>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>

#include <chrono>

using Miliseconds = std::chrono::milliseconds;

enum class EState
{
	Player1Won,
	Player2Won,
	Place,
	Move,
	Capture
};

enum class EOperationResult
{
	NoError,
	PieceIsPartOfALine,
	IsNotAValidPlace,
	IsNotAValidPiece,
};


class BoardWidget : public QWidget, public IClientListener
{
	Q_OBJECT

public:
	BoardWidget( bool vsComputer,const std::string& name, QWidget* parent=nullptr);
	~BoardWidget();

	///IClient listener functions
	void OnReset() override;
	void OnUndo(const std::vector<int>& positions,int state,int player) override;

	void OnWin(int state) override;
	void OnError(int error) override;
	void OnOpponentLeft() override;
	void OnServerDisconnected() override;

	void OnPlacePiece(Pos placePos, int color) override;
	void OnRemovePiece(Pos removePos) override;
	void OnMovePiece(Pos initialPos, Pos finalPos, int currentPlayer) override;
	void OnShowHints(const std::vector<int>& data) override;

	void OnUpdateGameVariables(int state, int currentPlayer) override;
	void OnGetClientPlayer(int player) override;
	void OnGetTime(const std::vector<int>& data) override;

	// QWidget methods
	void mousePressEvent(QMouseEvent* event) override;

	//Network
	void SetClient(IClientPtr client);

private slots:
	void OnResetButtonClicked();
	void OnUndoButtonClicked();
	void OnStrategyComboBoxClicked(int index);
	void OnOkButtonClicked();

protected:
	void paintEvent(QPaintEvent* event) override;

private:
	Pos FindIndexFromMatrix(int val);
	void ShowHints(Node* nod);

	Node* FindNodeFromInitialPoz(Pos position);
	Node* FindNodeFromScreenCoordinates(int x, int y);
	void TimeAction();

	//// utils
	static QString GameExceptionToString(int exception);
	static QString StateToString(int state);
	static QString FormatTime(Miliseconds ms);
	static QString FormatTimeAction(Miliseconds ms);

	//initialize
	void InitializeLayouts();
	void InitializeWidgets();
	void ConnectSignalsAndSlots();
	void InitializeBoard();
	void SetupBoardWidget();

private:
	std::vector<std::vector<int>> m_gameBoard;
	std::vector<Node*> m_nodes;

	Node* m_selectedNode1;
	Node* m_selectedNode2;

	bool m_strategyOn;
	int m_curentPlayer;
	int m_clientProxy;

	Miliseconds m_timeActionPlayer1;
	Miliseconds m_timeActionPlayer2;
	Miliseconds m_timeAction;

	Ui::BoardWidgetClass ui;

	QPushButton* resetButton;
	QPushButton* undoButton;

	QMessageBox m_msgBox;
	QMessageBox m_msgBoxEndGame;

	QVBoxLayout* m_layout1;
	QVBoxLayout* m_layout2;
	QHBoxLayout* m_playerLayout;
	QHBoxLayout* m_hLayout;

	QLabel* m_timePlayer1Label;
	QLabel* m_timePlayer2Label;
	QLabel* m_timeActionLabel;
	QLabel* m_playerToMove;
	QLabel* m_playerNetwork;
	QLabel* m_currentState;

	QComboBox* strategyComboBox;

	std::vector<std::vector<int>> m_nodeCoordinates;

	const std::string m_gameName;

	bool m_vsComputer;
	EState m_state;
	std::vector<std::pair<int, int>> m_hints;

	IClientPtr m_client;
};