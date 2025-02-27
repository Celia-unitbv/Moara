#include "Client.h"
#include "ECommand.h"

IClientPtr IClient::Produce()
{
	return std::make_shared<Client>();
}

Client::Client()
	: m_port(0)
	, m_state(0)
	, m_player(0)
{
	m_networkProvider = INetworkProvider::Produce(ENetworkProvider::Sfml);
}

void Client::ReceiveFromServer(const std::string& message)
{
	Command com;
	com.ParseFromArray(message.data(), message.size());
	switch (com.command())
	{
	case Ecommand::Place:
	{
		int x = com.place().x();
		int y = com.place().y();
		NotifyListenersOnPlace({ x, y }, com.place().player());
		break;
	}

	case Ecommand::Capture:
	{
		int x = com.capture().x();
		int y = com.capture().y();
		NotifyListenersOnRemove({ x, y });
		break;
	}

	case Ecommand::Move:
	{
		int from_x = com.move().from_x();
		int to_x = com.move().to_x();
		int from_y = com.move().from_y();
		int to_y = com.move().to_y();
		NotifyListenersOnMove({ from_x, from_y }, { to_x, to_y }, com.move().player());
		break;
	}

	case Ecommand::WhereToMove:
	{
		std::vector<int> hints;
		const WhereToMovePiece& whereToMoveData = com.wheretomovepiece();
		for (const auto& pair : whereToMoveData.pairs())
		{
			hints.push_back(pair.first());
			hints.push_back(pair.second());
		}
		NotifyListenersOnWhereToMove(hints);
		break;
	}

	case Ecommand::Reset:
	{
		NotifyListenersOnReset();
		break;
	}

	case Ecommand::Time:
	{
		std::vector<int> times;
		times.push_back(com.gettime().timeplayer1());
		times.push_back(com.gettime().timeplayer2());
		times.push_back(com.gettime().timeaction());
		NotifyListenersOnTime(times);
		break;
	}

	case Ecommand::ProxyPlayer:
	{
		NotifyListenersOnClientPlayer(com.getproxyplayer().player());
		break;
	}

	case Ecommand::Undo:
	{
		std::vector<int> data;
		int state = com.undoaction().state();
		int player = com.undoaction().player();
		for (const auto& pair : com.undoaction().position())
		{
			data.push_back(pair.first());
			data.push_back(pair.second());
		}
		NotifyListenersOnUndo(data, state, player);
		break;
	}

	case Ecommand::StartLobby:
	{
		NotifyListenersOnStart();
		break;
	}

	case Ecommand::ServerDisconnect:
	{
		NotifyListenersOnServerDisconnected();
		break;
	}

	case Ecommand::OpponentDisconnect:
	{
		NotifyListenersOnOpponetLeft();
		break;
	}

	case Ecommand::GameAlreadyExists:
	{
		NotifyListenersGameAlreadyExists();
		break;
	}

	case Ecommand::Win:
	{
		NotifyListenersOnWin(com.getwinner().state());
		break;
	}

	case Ecommand::Error:
	{
		NotifyListenersOnError(com.geterror().error());
		break;
	}

	case Ecommand::State:
	{
		m_state = com.getstate().state();
		m_player = com.getstate().player();
		NotifyListenersOnGameVariablesUpdate();
		break;
	}

	default:
	{
		break;
	}
	}

	
}

void Client::PlacePiece(const Pos& placePos)
{
	std::string position = m_messageHelper.GetMessageForPlacePiece(placePos);
	m_networkProvider->SendMessage(m_messageHelper.GetMessageForServer(m_gameName, position));
}

void Client::CapturePiece(const Pos& capturePos)
{
	std::string position = m_messageHelper.GetMessageForCapturePiece(capturePos);
	m_networkProvider->SendMessage(m_messageHelper.GetMessageForServer(m_gameName, position));
}

void Client::MovePiece(const Pos& initialPoz, const Pos& finalPoz)
{
	std::string position = m_messageHelper.GetMessageForMovePiece(initialPoz,finalPoz);
	m_networkProvider->SendMessage(m_messageHelper.GetMessageForServer(m_gameName, position));
}

void Client::EndGame()
{
	for (auto it : m_listeners)
	{
		RemoveListener(it);
	}
	m_networkProvider->DisconnectFromServer();
}

void Client::Undo()
{
	std::string position = m_messageHelper.GetMessageForUndo();
	m_networkProvider->SendMessage(m_messageHelper.GetMessageForServer(m_gameName, position));
}

void Client::StartGame()
{
	m_networkProvider->SetMessageReceivedCb(std::bind(&Client::ReceiveFromServer, this, std::placeholders::_1));
	std::string position = m_messageHelper.GetMessageForStart();
	m_networkProvider->SendMessage(m_messageHelper.GetMessageForServer(m_gameName, position));
}

void Client::Reset()
{
	std::string position = m_messageHelper.GetMessageForReset();
	m_networkProvider->SendMessage(m_messageHelper.GetMessageForServer(m_gameName, position));
}

void Client::SetConnectionInfo(const std::string& host, int port)
{
	m_port = port;
	m_host = host;
}

void Client::SetGameName(const std::string& gameName)
{
	m_gameName = gameName;
}

void Client::AddListener(IClientListener* listener)
{
	m_listeners.emplace_back(listener);
}

void Client::RemoveListener(IClientListener* listener)
{
	auto func = [listener](IClientListener* el)
	{
		return el == listener;
	};

	m_listeners.erase(std::remove_if(m_listeners.begin(), m_listeners.end(), func));
}

void Client::NotifyListenersOnPlace(const Pos position, int color)
{
	for (auto it : m_listeners)
	{
		it->OnPlacePiece(position, color);
	}
}

void Client::NotifyListenersOnUpdateState(int state, int currenPlayer)
{
	for (auto it : m_listeners)
	{
		it->OnUpdateGameVariables(state, currenPlayer);
	}
}

void Client::NotifyListenersOnClientPlayer(int player)
{
	for (auto it : m_listeners)
	{
		it->OnGetClientPlayer(player);
	}
}

void Client::NotifyListenersOnMove(const Pos& initialPoz, const Pos& finalPoz, int color)
{
	for (auto it : m_listeners)
	{
		it->OnMovePiece(initialPoz,finalPoz, color);
	}
}

void Client::NotifyListenersOnRemove(const Pos position)
{
	for (auto it : m_listeners)
	{
		it->OnRemovePiece(position);
	}
}

void Client::NotifyListenersOnWin(int state)
{
	for (auto it : m_listeners)
	{
		it->OnWin(state);
	}
}

void Client::NotifyListenersOnError(int error)
{
	for (auto it : m_listeners)
	{
		it->OnError(error);
	}
}

void Client::NotifyListenersOnUndo(const std::vector<int>& data,int state,int player)
{
	for (auto it : m_listeners)
	{
		it->OnUndo(data,state,player);
	}
}

void Client::NotifyListenersOnWhereToMove(const std::vector<int>& data)
{
	for (auto it : m_listeners)
	{
		it->OnShowHints(data);
	}
}

void Client::NotifyListenersOnTime(const std::vector<int>& data)
{
	for (auto it : m_listeners)
	{
		it->OnGetTime(data);
	}
}

void Client::NotifyListenersOnReset()
{
	for (auto it : m_listeners)
	{
		it->OnReset();
	}
}

void Client::NotifyListenersOnOpponetLeft()
{
	for (auto it : m_listeners)
	{
		it->OnOpponentLeft();
	}
}

void Client::NotifyListenersOnServerDisconnected()
{
	for (auto it : m_listeners)
	{
		it->OnServerDisconnected();
	}
}

void Client::NotifyListenersOnStart()
{
	for (auto it : m_listeners)
	{
		it->OnStart();
	}
}

void Client::NotifyListenersGameAlreadyExists()
{
	for (auto it : m_listeners)
	{
		it->OnGameAlreadyExists();
	}
}

void Client::NotifyListenersOnGameVariablesUpdate()
{
	for (auto it : m_listeners)
	{
		it->OnUpdateGameVariables(m_state,m_player);
	}
}

void Client::GetGameState()
{
	std::string position = m_messageHelper.GetMessageForGameState();
	m_networkProvider->SendMessage(m_messageHelper.GetMessageForServer(m_gameName, position));
	
}

void Client::GetTime()
{
	std::string position = m_messageHelper.GetMessageForTime();
	m_networkProvider->SendMessage(m_messageHelper.GetMessageForServer(m_gameName, position));
}

void Client::GetClientPlayer()
{
	std::string position = m_messageHelper.GetMessageForProxyPlayer();
	m_networkProvider->SendMessage(m_messageHelper.GetMessageForServer(m_gameName, position));
}

void Client::SetStrategy(int startegy)
{
	std::string position = m_messageHelper.GetMessageForStrategy(startegy);
	m_networkProvider->SendMessage(m_messageHelper.GetMessageForServer(m_gameName, position));
}

void Client::ShowHints(const Pos& position)
{
	std::string msg = m_messageHelper.GetMessageForWhereToMove(position);
	m_networkProvider->SendMessage(m_messageHelper.GetMessageForServer(m_gameName, msg));
}

bool Client::Start( bool vsComputer, const std::string& gameName)
{
	if (m_networkProvider->ConnectToServer(m_host,m_port))
	{
		std::string game;
		if (vsComputer)
			game = "robot" + gameName;
		else
			game = gameName;
		SetGameName(game);
		std::string message = "join," + game;

		if (m_networkProvider->SendMessage(message))
		{
			m_networkProvider->SetMessageReceivedCb(std::bind(&Client::ReceiveFromServer, this, std::placeholders::_1));
			return true;
		}
	}
	return false;
}