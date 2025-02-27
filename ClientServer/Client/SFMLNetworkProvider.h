#pragma once

#include "INetworkProvider.h"

#include "SFML/Network.hpp"

#include <thread>

class SfmlNetworkProvider : public INetworkProvider
{
public:
	SfmlNetworkProvider();

	bool ConnectToServer(const std::string& host, int port);
	void DisconnectFromServer();

	void SetMessageReceivedCb(MessageReceivedCallback callback);

	bool SendMessage(const std::string& message);

private:
	void Process();

private:
	sf::TcpSocket m_socket;

	MessageReceivedCallback m_messageReceivedCb;

	std::thread m_thread;
	std::atomic<bool> m_running;
};