#pragma once

#include "IGame.h"
#include "Node.h"
#include "Game.h"
#include "ui_BoardWidget.h"

#include <QWidget>
#include <QMouseEvent>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>

class BoardWidget : public QWidget, public IGameListener
{
	Q_OBJECT

public:
	BoardWidget(QWidget* parent = nullptr);
	~BoardWidget();

	// IGameListener methods
	void OnPlacePiece(Pos placePos, int color) override;
	void OnRemovePiece(Pos removePos) override;
	void OnLoad() override;
	void OnReset() override;
	void OnSave() override;
	void OnUndo(const Positions& position) override;
	void OnMovePiece(Pos initialPos, Pos finalPos, int m_currentPlayer) override;
	void OnWin() override;

	// QWidget methods
	void mousePressEvent(QMouseEvent* event) override;

	// other methods
	void SetGame(IGamePtr game);

private slots:
	void OnSaveButtonClicked();
	void OnLoadButtonClicked();
	void OnResetButtonClicked();
	void OnUndoButtonClicked();
	void OnStrategyComboBoxClicked(int index);
	void onOkButtonClicked();

protected:
	void paintEvent(QPaintEvent* event) override;

private:
	Pos FindIndexFromMatrix(int val);
	std::vector<Node*> WhereToMove(Node* nod);

	Node* FindNodeFromInitialPoz(Pos position);
	Node* FindNodeFromScreenCoordinates(int x, int y);

	void TimeAction(EPlayer player);

	// utils
	static QString GameExceptionToString(EOperationResult exception);
	static QString StateToString(EState state);
	static QString FormatTime(Miliseconds ms);
	static QString FormatTimeAction(Miliseconds ms);

	//initialize
	void initializeLayouts();
	void initializeWidgets();
	void connectSignalsAndSlots();
	void initializeBoard();
	void setupBoardWidget();

private:
	IGamePtr m_game;

	std::vector<std::vector<int>> m_gameBoard;
	std::vector<Node*> m_nodes;

	Node* m_selectedNode1;
	Node* m_selectedNode2;

	bool m_strategyOn;

	Miliseconds m_timeActionPlayer1;
	Miliseconds m_timeActionPlayer2;

	Ui::BoardWidgetClass ui;

	QPushButton* saveButton;
	QPushButton* loadButton;
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
	QLabel* m_player;

	QComboBox* strategyComboBox;

	std::vector<std::vector<int>> m_nodeCoordinates;
};