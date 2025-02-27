#pragma once

#include "IGame.h"
#include "IStrategy.h"
#include "Board.h"
#include "Timer.h"
#include "Random.h"

#include <string>
#include <list>

struct GameHistory 
{
	EState m_state;
	Positions m_piece;
};

class Game : public IGame
{
public:
	Game(bool vsComputer = false, bool testing=false);

	// IGame methods
	EOperationResult PlacePiece(Pos placePos) override;
	EOperationResult RemovePiece(Pos removePos) override;
	EOperationResult MovePiece(Pos initialPos, Pos finalPos) override;
	EOperationResult MakeComputerAction();

	void AddListener(IGameListener* listener)override;
	void RemoveListener(IGameListener* listener)override;

	EPlayer GetCurrentPlayer() const override;
	EState GetGameState() const override;
	const BoardMatrix& GetBoard() const override;
	const bool GetGameVsComputer() const override;

	bool SaveGame(const std::string& filename, const std::string& historyfile) const override;
	bool LoadGame(const std::string& filename) override;

	bool Undo() override;
	void ResetGame() override;

	void SetStrategy(EGameType type);
	void SetStrategy(IStrategyPtr strategy);

	Miliseconds GetTime(EPlayer player) override;

	void Start() override;
	

	// other methods

	//Timer methods
	Miliseconds GetTimeAction() override;
	void Stop();

	//Hints for action
	Positions WhereToMove(Pos position) const override;
	Positions WhereToCapture() const override;



private:
	bool CheckLine(Pos position) const;

	void LoadBoard(std::vector<std::vector<int>> BoardColors);

	void SwitchPlayers();

	IPiecePtr CreatePieceColor(int x, int y, int color);
	IPiecePtr CreatePiece(Pos PieceCoordinates) const;

	void AddAction(EState state, const std::vector < std::pair<int, int>>& pieces);

	//Notify 
	void NotifyListenersPlacePiece(Pos placePos) const;
	void NotifyListenersRemovePiece(Pos removePos) const;
	void NotifyListenersMovePiece(Pos initialPos, Pos finalPos) const;
	void NotifyListenersOnUndo(Positions pieces) const;
	void NotifyListenersOnLoad() const ;
	void NotifyListenersOnReset() const;
	void NotifyListenersOnSave() const;
	void NotifyListenersOnWin() const;
	void NotifyListenersOnError(int result) const;

private:
	bool m_vsComputer;
	bool m_firstMove;
	bool m_testing;
	
	EState m_GameState;
	Board m_Board;
	EPlayer m_currentPlayer;
	
	GameListenerList m_Listeners;
	
	IStrategyPtr m_strategy;
	
	std::list<GameHistory> m_gameHistory;

	Timer m_timer;
	Miliseconds m_timeAction;

	RandomGenerator m_randomGenerator;

};
