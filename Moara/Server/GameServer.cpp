#include "GameServer.h"

#include <iostream>
#include <sstream>
#include <regex>


GameServer::GameServer()
{
	m_networkProvider = INetworkProvider::Produce(ENetworkProvider::Sfml);

	m_networkProvider->SetClientMessageHandler(std::bind(&GameServer::HandleClientMessage, this, std::placeholders::_1, std::placeholders::_2));
	
	m_networkProvider->SetDisconnectionHandler(std::bind(&GameServer::HandleClientDisconnection, this, std::placeholders::_1));

	m_networkProvider->SetConnectionHandler(std::bind(&GameServer::HandleClientConnection, this, std::placeholders::_1));
}

bool GameServer::Init(int port)
{
	return m_networkProvider->Init(port);
}

void GameServer::Run()
{
	m_networkProvider->Run();
}

void GameServer::HandleClientMessage(int clientId, const std::string& message)
{
	std::string command, gameName, gameMessage;

	std::vector<std::string> data = SplitStringToStrings(message);
	command = data[0];
	gameName = data[1];

	if (command == "join")
	{
		if (CheckGameAlreadyExists(gameName))
		{
			Command command;
			command.set_command(Ecommand::GameAlreadyExists);
			std::string serialized_command;
			command.SerializeToString(&serialized_command);
			m_networkProvider->SendToClient(clientId, serialized_command);
			return;
		}
		else
		{
			std::regex robotRegex("^robot.*");
			if (std::regex_match(gameName, robotRegex))
			{
				activeGames[gameName] = IGame::Produce(true);

				clientClientProxy[clientId]->SetBroadcastCallback(std::bind(&GameServer::BroadcastMessageToGame, this, std::placeholders::_1, gameName));
				clientClientProxy[clientId]->SetGame(activeGames[gameName]);
				clientClientProxy[clientId]->SetNetworkProvider(m_networkProvider);
				clientClientProxy[clientId]->SetPlayer(EPlayer::Player1);
				clientClientProxy[clientId]->SetGameName(gameName);

				lobbies[gameName].push_back(clientId);

				activeGames[gameName]->AddListener(clientClientProxy[clientId]);

				Command command;
				command.set_command(Ecommand::StartLobby);
				std::string serialized_command;
				command.SerializeToString(&serialized_command);

				m_networkProvider->SendToClient(clientId, serialized_command);
			}
			else if ( lobbies[gameName].size() < 2)
			{
				lobbies[gameName].push_back(clientId);
				clientClientProxy[clientId]->SetGameName(gameName);

				StartGameIfReady(gameName);
			}
		}
	}
	else if (command == "message")
	{
		gameMessage = data[2];

		if (clientClientProxy.find(clientId) != clientClientProxy.end())
		{
			auto& clientProxy = clientClientProxy[clientId];
			clientProxy->HandleClient(gameMessage);
		}
		else
		{
			std::cerr << "Error: Client not found " << std::endl;
		}
	}
}

void GameServer::HandleClientDisconnection(int clientId)
{
	auto clientProxy = clientClientProxy[clientId];

	std::string gameName = clientProxy->GetGameName();
	std::regex robotRegex("^robot.*");

	if(!std::regex_match(gameName, robotRegex) && activeGames[gameName])
		activeGames[gameName]->RemoveListener(clientProxy);

	auto& lobby = lobbies[gameName];
	lobby.erase(std::remove(lobby.begin(), lobby.end(), clientId), lobby.end());

	delete clientClientProxy[clientId];
	clientClientProxy.erase(clientId);
	
	if (lobby.empty())
	{
		lobbies.erase(gameName);
		activeGames.erase(gameName);
	}
	else
	{
		Command command;
		command.set_command(Ecommand::OpponentDisconnect);
		std::string serialized_command;
		command.SerializeToString(&serialized_command);
		m_networkProvider->SendToClient(lobby[0], serialized_command);
	}

	std::cout << "Client disconnected, game: " << gameName << std::endl;
}

void GameServer::HandleClientConnection(int clientId)
{
	ClientProxy* proxy = new ClientProxy(clientId);
	clientClientProxy.insert({ clientId,proxy });
}

std::vector<std::string> GameServer::SplitStringToStrings(const std::string& str)
{
	std::vector<std::string> result;
	std::istringstream iss(str);
	std::string temp;

	while (std::getline(iss, temp, ',')) 
	{
		result.push_back(temp);
	}

	return result;
}

void GameServer::StartGameIfReady(const std::string& gameName)
{
	if (lobbies[gameName].size() == 2)
	{
		EPlayer player = EPlayer::Player1;
		activeGames[gameName] = IGame::Produce(false);

		for (auto client : lobbies[gameName])
		{
			clientClientProxy[client]->SetGame(activeGames[gameName]);
			clientClientProxy[client]->SetBroadcastCallback(std::bind(&GameServer::BroadcastMessageToGame, this, std::placeholders::_1, gameName));
			clientClientProxy[client]->SetNetworkProvider(m_networkProvider);
			clientClientProxy[client]->SetPlayer(player);

			player = player == EPlayer::Player1 ? EPlayer::Player2 : EPlayer::Player1;

			activeGames[gameName]->AddListener(clientClientProxy[client]);

			Command command;
			command.set_command(Ecommand::StartLobby);
			std::string serialized_command;
			command.SerializeToString(&serialized_command);

			m_networkProvider->SendToClient(client, serialized_command);
		}
	}
}

void GameServer::BroadcastMessageToGame(const std::string& message, const std::string& gameName)
{
	auto& lobby = lobbies[gameName];
	for (auto clientId : lobby)
	{
		m_networkProvider->SendToClient(clientId, message);
	}
}

bool GameServer::CheckGameAlreadyExists(const std::string& gameName)
{
	for (auto game : activeGames)
	{
		if (game.first == gameName) 
		{
			if (game.second->GetGameVsComputer())
				return true;
			else
			{
				auto& lobby = lobbies[gameName];
				if (lobby.size() == 2)
				{
					return true;
				}
			}
		}
	}
	return false;
}