#pragma once

#include "INetworkProvider.h"

#include <SFML/Network.hpp>

#include <unordered_map>

class SFMLNetworkProvider: public INetworkProvider
{
public:
	SFMLNetworkProvider();

	bool Init(int port) override;
	void Run() override;

	bool SendToClient(int clientId, const std::string& message) override;

	void SetClientMessageHandler(MessageReceivedCallback handler) override;
	void SetDisconnectionHandler(ConnectionDisconnectionCallback handler) override;
	void SetConnectionHandler(ConnectionDisconnectionCallback handler) override;

private:
	void AcceptNewClient();
	void ReceiveFromClients();

private:
	sf::TcpListener listener;
	sf::SocketSelector selector;

	std::map<int, sf::TcpSocket*> clients;
	int m_clientId;

	MessageReceivedCallback clientMessageHandler;
	ConnectionDisconnectionCallback disconnectionHandler;
	ConnectionDisconnectionCallback connectionHandler;
};