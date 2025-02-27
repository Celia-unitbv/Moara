#pragma once

#include "IGameListener.h"
#include"EPlayer.h"
#include "IGame.h"
#include "MessageHelper.h"
#include "INetworkProvider.h"

using BroadcastMessageCallback = std::function<void(const std::string&, const std::string&)>;


class ClientProxy : public IGameListener
{
public:
	ClientProxy(int id);
	~ClientProxy()=default;

	// Inherited via IGameListener
	 void OnPlacePiece(Pos placePos, int color) override;
	 void OnRemovePiece(Pos removePos) override;
	 void OnMovePiece(Pos initialPos, Pos finalPos, int currentPlayer) override;
	 void OnLoad() override;
	 void OnReset() override;
	 void OnSave() override;
	 void OnUndo(const Positions& positions) override;
	 void OnWin() override;
	 void OnError(int result) override;

	//Setters
	void SetNetworkProvider(INetworkProvider* provider);
	void SetBroadcastCallback(BroadcastMessageCallback broadcastMessage);
	void SetGameName(const std::string& name);
	void SetGame(IGamePtr game);
	void SetPlayer(EPlayer player);

	//Getters
	int GetId() const;
	const std::string GetGameName()const;

	//Network
	void HandleClient(const std::string& message);
	void Broadcast(const std::string& message);

private:

	int m_id;

	EPlayer m_player;

	IGamePtr m_game;
	std::string m_gameName;

	INetworkProvider* m_networkProvider;
	BroadcastMessageCallback broadcastMessage;

	MessageHelper m_messageHelper;
	std::string messageForClient;
};

