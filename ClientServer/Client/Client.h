#pragma once

#include "IClient.h"
#include "IClientListener.h"
#include "MessageHelper.h"
#include "INetworkProvider.h"

class Client: public IClient
{
public:
	Client();

	//IClient methods
	void PlacePiece(const Pos& placePos) override;
	void CapturePiece(const Pos& placePos) override;
	void MovePiece(const Pos& initialPoz, const Pos& finalPoz) override;
	void ShowHints(const Pos& position) override;

	void EndGame() override;
	void StartGame() override;

	void Undo() override;
	void Reset() override;

	void GetGameState() override;
	void GetTime() override;
	void GetClientPlayer() override;

	void SetStrategy(int startegy) override;
	void SetConnectionInfo(const std::string& host, int port);
	void SetGameName(const std::string& gameName) override;

	bool Start(bool vsComputer, const std::string& gameName) override;

	void AddListener(IClientListener* listener) override;
	void RemoveListener(IClientListener* listener) override;

private:
	//Notify
	void NotifyListenersOnPlace(const Pos position, int color);
	void NotifyListenersOnUpdateState(int state, int currenPlayer);
	void NotifyListenersOnClientPlayer(int player);
	void NotifyListenersOnMove(const Pos& initialPoz, const Pos& finalPoz, int color);
	void NotifyListenersOnRemove(const Pos position);
	void NotifyListenersOnWin(int state);
	void NotifyListenersOnError(int error);
	void NotifyListenersOnUndo(const std::vector<int>& data,int state,int player);
	void NotifyListenersOnWhereToMove(const std::vector<int>& data);
	void NotifyListenersOnTime(const std::vector<int>& data);
	void NotifyListenersOnReset();
	void NotifyListenersOnOpponetLeft();
	void NotifyListenersOnServerDisconnected();
	void NotifyListenersOnStart(); 
	void NotifyListenersGameAlreadyExists();
	void NotifyListenersOnGameVariablesUpdate();

	//Network
	void ReceiveFromServer(const std::string& message);

private:

	MessageHelperClient m_messageHelper;
	INetworkProvider* m_networkProvider;
	ClientListenerList m_listeners;

	std::string m_gameName;
	std::string m_host;
	int m_port;

	int m_state;
	int m_player;
};

