#include "ClientProxy.h"

#include "GameServer.h"
#include "ECommand.h"


ClientProxy::ClientProxy(int id)
	: m_id(id)
	, m_game(nullptr)
	, m_player(EPlayer::Player1)
	, m_networkProvider(nullptr)
{
	
}

void ClientProxy::SetNetworkProvider(INetworkProvider* provider)
{
	m_networkProvider = provider;
}

void ClientProxy::SetBroadcastCallback(BroadcastMessageCallback broadcastCallback)
{
	broadcastMessage = broadcastCallback;
}

void ClientProxy::SetGameName(const std::string& name)
{
	m_gameName = name;
}

void ClientProxy::SetGame(IGamePtr game)
{
	m_game = game;
	m_messageHelper.SetGame(m_game);
}

void ClientProxy::SetPlayer(EPlayer player)
{
	m_player = player;
}

int ClientProxy::GetId() const
{
	return m_id;
}

const std::string ClientProxy::GetGameName() const
{
	return m_gameName;
}

void ClientProxy::OnPlacePiece(Pos placePos, int color)
{
    messageForClient =m_messageHelper.GetMessageForPlacePiece(placePos, color);
	m_networkProvider->SendToClient(m_id, messageForClient);
}

void ClientProxy::OnRemovePiece(Pos removePos)
{					
    messageForClient = m_messageHelper.GetMessageForCapturePiece(removePos);
	m_networkProvider->SendToClient(m_id, messageForClient);
}

void ClientProxy::OnMovePiece(Pos initialPos, Pos finalPos, int currentPlayer)
{							
    messageForClient = m_messageHelper.GetMessageForMovePiece(initialPos, finalPos, currentPlayer);
	m_networkProvider->SendToClient(m_id, messageForClient);
}

void ClientProxy::OnLoad()
{

}

void ClientProxy::OnReset()
{
    messageForClient = m_messageHelper.GetMessageForReset();
	m_networkProvider->SendToClient(m_id, messageForClient);
}

void ClientProxy::OnSave()
{

}

void ClientProxy::OnUndo(const Positions& positions)
{
    messageForClient =m_messageHelper.GetMessageForundo(positions);
	m_networkProvider->SendToClient(m_id, messageForClient);
}

void ClientProxy::OnWin()
{
    messageForClient = m_messageHelper.GetMessageForWin();
	m_networkProvider->SendToClient(m_id, messageForClient);
}

void ClientProxy::OnError(int result)
{
    messageForClient = m_messageHelper.GetMessageForError(result);
	m_networkProvider->SendToClient(m_id, messageForClient);
}

void ClientProxy::HandleClient(const std::string& message)
{
	Command com;
	com.ParseFromArray(message.data(), message.size());

    switch (com.command())
    {
    case Ecommand::Place:
    {
        const PlacePiece& place = com.place();
        int x = place.x();
        int y = place.y();
        std::pair<int, int> position = std::make_pair(x, y);

        if (m_player == m_game->GetCurrentPlayer())
        {
            auto currentPlayer = m_game->GetCurrentPlayer();
            auto result = m_game->PlacePiece(position);

            if (m_game->GetGameState() == EState::Capture)
            {
                messageForClient = m_messageHelper.GetMessageForWhereToCapture();
                m_networkProvider->SendToClient(m_id, messageForClient);
            }
        }
        break;
    }

    case Ecommand::Capture:
    {
        const CapturePiece& place = com.capture();
        int x = place.x();
        int y = place.y();
        std::pair<int, int> positionInitial = std::make_pair(x, y);

        if (m_player == m_game->GetCurrentPlayer())
        {
            m_game->RemovePiece(positionInitial);
        }
        break;
    }

    case Ecommand::Move:
    {
        const MovePiece& place = com.move();
        int fromx = place.from_x();
        int fromy = place.from_y();
        int tox = place.to_x();
        int toy = place.to_y();

        std::pair<int, int> positionFrom = std::make_pair(fromx, fromy);
        std::pair<int, int> positionTo = std::make_pair(tox, toy);

        if (m_player == m_game->GetCurrentPlayer())
        {
            auto currentPlayer = m_game->GetCurrentPlayer();

            m_game->MovePiece(positionFrom, positionTo);

            if (m_game->GetGameState() == EState::Capture)
            {
                messageForClient = m_messageHelper.GetMessageForWhereToCapture();
                m_networkProvider->SendToClient(m_id, messageForClient);
            }
        }
        break;
    }

    case Ecommand::WhereToMove:
    {
        const WhereToMovePiece& place = com.wheretomovepiece();
        int x = place.x();
        int y = place.y();
        std::pair<int, int> position = std::make_pair(x, y);

        messageForClient = m_messageHelper.GetMessageForWhereToMove(position);
        m_networkProvider->SendToClient(m_id, messageForClient);
        break;
    }

    case Ecommand::Reset:
    {
        if (m_player == m_game->GetCurrentPlayer())
        {
            m_game->ResetGame();
        }
        break;
    }

    case Ecommand::Time:
    {
        messageForClient = m_messageHelper.GetMessageForAllTime();
        m_networkProvider->SendToClient(m_id, messageForClient);
        break;
    }

    case Ecommand::ProxyPlayer:
    {
        messageForClient = m_messageHelper.GetMessageForClientPlayer(m_player);
        m_networkProvider->SendToClient(m_id, messageForClient);
        break;
    }

    case Ecommand::Strategy:
    {
        int difficulty = com.setstrategy().difficulty();
        if (difficulty == 0)
        {
            m_game->SetStrategy(EGameType::Easy);
        }
        else
        {
            m_game->SetStrategy(EGameType::Medium);
        }
        break;
    }

    case Ecommand::Undo:
    {
        if (m_player != m_game->GetCurrentPlayer())
        {
            m_game->Undo();
        }
        break;
    }

    case Ecommand::StartGame:
    {
        m_game->Start();
        break;
    }

    case Ecommand::State:
    {
        messageForClient = m_messageHelper.GetMessageForGameState();
        Broadcast(messageForClient);
        break;
    }

    default:
    {
        break;
    }
    }
}

void ClientProxy::Broadcast(const std::string& message)
{
	if (broadcastMessage)
	{
		broadcastMessage(message, m_gameName);
	}
}