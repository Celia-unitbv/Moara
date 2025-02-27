#pragma once

#include "ClientProxy.h"

#include <vector>
#include <unordered_map>
#include <string>

class GameServer
{
public:
    GameServer();

    bool Init(int port);

    void Run();

private:
	void HandleClientMessage(int clientId, const std::string& message);
	void HandleClientDisconnection(int clientId);
	void HandleClientConnection(int clientId);

    static std::vector<std::string> SplitStringToStrings(const std::string& str);

    void StartGameIfReady(const std::string& gameName);
    void BroadcastMessageToGame(const std::string& message, const std::string& gameName);
    bool CheckGameAlreadyExists(const std::string& gameName);

private:
    std::unordered_map<int, ClientProxy*> clientClientProxy;
    std::unordered_map<std::string, std::vector<int>> lobbies;
    std::unordered_map<std::string, IGamePtr> activeGames;

    INetworkProvider* m_networkProvider;
};


