#pragma once

#include <vector>

using Pos = std::pair<int, int>;
class IClientListener
{
public:
	//Piece actions
	virtual void OnPlacePiece(Pos placePos, int color) {};
	virtual void OnRemovePiece(Pos removePos) {};
	virtual void OnMovePiece(Pos initialPos, Pos finalPos, int currentPlayer) {};
	virtual void OnShowHints(const std::vector<int>& data) {};
	virtual void OnStart(){};
	virtual void OnGameAlreadyExists() {};

	//Buttons
	virtual void OnReset() {};
	virtual void OnUndo(const std::vector<int>& positions,int state,int player) {};

	virtual void OnWin(int state) {};
	virtual void OnError(int error) {};

	//Network
	virtual void OnOpponentLeft() {};
	virtual void OnServerDisconnected() {};

	//getters
	virtual void OnUpdateGameVariables(int state, int currentPlayer) {};
	virtual void OnGetClientPlayer(int player) {};
	virtual void OnGetTime(const std::vector<int>& data) {};

	virtual ~IClientListener() = default;
};

using ClientListenerList = std::vector<IClientListener*>;