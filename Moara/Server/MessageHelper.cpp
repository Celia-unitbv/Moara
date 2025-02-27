#include "MessageHelper.h"

#include <sstream>

MessageHelper::MessageHelper()
{
	command = new Command();
}

void MessageHelper::SetGame(IGamePtr game)
{
	m_game = game;
}

const std::string MessageHelper::GetMessageForPlacePiece(Pos pozition, int color) const
{
	command->set_command(Ecommand::Place);

	PlacePiece* place_data = command->mutable_place();
	place_data->set_x(pozition.first);
	place_data->set_y(pozition.second);
	place_data->set_player(color);

	std::string serialized_command;
	command->SerializeToString(&serialized_command);
	command->Clear();

	return serialized_command;
}

const std::string MessageHelper::GetMessageForMovePiece(Pos initialPos, Pos finalPos, int currentPlayer) const
{
	command->set_command(Ecommand::Move);

	MovePiece* place_data = command->mutable_move();
	place_data->set_from_x(initialPos.first);
	place_data->set_from_y(initialPos.second);
	place_data->set_to_x(finalPos.first);
	place_data->set_to_y(finalPos.second);
	place_data->set_player(currentPlayer);

	std::string serialized_command;
	command->SerializeToString(&serialized_command);
	command->Clear();
	return serialized_command;

}

const std::string MessageHelper::GetMessageForCapturePiece(Pos pozition) const
{
	command->set_command(Ecommand::Capture);

	CapturePiece* place_data = command->mutable_capture();
	place_data->set_x(pozition.first);
	place_data->set_y(pozition.second);


	std::string serialized_command;
	command->SerializeToString(&serialized_command);

	return serialized_command;
}

const std::string MessageHelper::GetMessageForLoad() const
{
	std::stringstream ss;
	ss << 8 << " " << 1 << " ";
	int color;

	for (const auto& row : m_game->GetBoard())
	{
		for (const auto& piece : row)
		{
			if (piece)
				color = (int)(piece->GetColor());
			else
				color = -1;

			ss << color << " ";
		}
	}

	ss << "1";
	return ss.str();
}

const std::string MessageHelper::GetMessageForError(int result) const
{
	command->set_command(Ecommand::Error);

	GetError* getError = command->mutable_geterror();
	getError->set_error(result);

	std::string serialized_command;
	command->SerializeToString(&serialized_command);

	return serialized_command;
}

const std::string MessageHelper::GetMessageForWin() const
{
	command->set_command(Ecommand::Win);

	GetWinner* getWinner = command->mutable_getwinner();
	getWinner->set_state((int)m_game->GetGameState());

	std::string serialized_command;
	command->SerializeToString(&serialized_command);

	return serialized_command;
}

const std::string MessageHelper::GetMessageForReset() const
{
	command->set_command(Ecommand::Reset);

	std::string serialized_command;
	command->SerializeToString(&serialized_command);
	command->Clear();
	return serialized_command;
}

const std::string MessageHelper::GetMessageForWhereToCapture() const
{
	command->set_command(Ecommand::WhereToMove);

	WhereToMovePiece* place_data = command->mutable_wheretomovepiece();

	Positions move = m_game->WhereToCapture();

	for (int i = 0; i < move.size(); ++i)
	{
		IntPair* pair = place_data->add_pair();
		pair->set_first(move[i].first);
		pair->set_second(move[i].second);
	}

	std::string serialized_command;
	command->SerializeToString(&serialized_command);

	return serialized_command;
}

const std::string MessageHelper::GetMessageForWhereToMove(const Pos position) const
{
	command->set_command(Ecommand::WhereToMove);

	WhereToMovePiece* place_data = command->mutable_wheretomovepiece();

	Positions move = m_game->WhereToMove(position);
	
	for (int i = 0; i < move.size(); ++i)
	{
		IntPair* pair = place_data->add_pair();
		pair->set_first(move[i].first);
		pair->set_second(move[i].second);
	}

	std::string serialized_command;
	command->SerializeToString(&serialized_command);

	return serialized_command;
}

const std::string MessageHelper::GetMessageForGameState() const
{
	command->set_command(Ecommand::State);

	GetState* getState = command->mutable_getstate();
	getState->set_player((int)m_game->GetCurrentPlayer());
	getState->set_state((int)m_game->GetGameState());

	std::string serialized_command;
	command->SerializeToString(&serialized_command);

	return serialized_command;
}

const std::string MessageHelper::GetMessageForClientPlayer(EPlayer m_player) const
{
	command->set_command(Ecommand::ProxyPlayer);

	GetProxyPlayer* proxy = command->mutable_getproxyplayer();
	proxy->set_player((int)m_player);

	std::string serialized_command;
	command->SerializeToString(&serialized_command);

	return serialized_command;
}

const std::string MessageHelper::GetMessageForAllTime() const
{
	int time = static_cast<int>(m_game->GetTime(EPlayer::Player1).count());
	int time2 = static_cast<int>(m_game->GetTime(EPlayer::Player2).count());
	int time3 = static_cast<int>(m_game->GetTimeAction().count());

	command->set_command(Ecommand::Time);

	GetTime* getTime = command->mutable_gettime();
	getTime->set_timeaction(time3);
	getTime->set_timeplayer1(time);
	getTime->set_timeplayer2(time2);

	std::string serialized_command;
	command->SerializeToString(&serialized_command);

	return serialized_command;
}

const std::string MessageHelper::GetMessageForundo(const Positions& positions) const
{
	command->set_command(Ecommand::Undo);

	UndoAction* undoAction = command->mutable_undoaction();
	undoAction->set_state((int)m_game->GetGameState());
	undoAction->set_player((int)m_game->GetCurrentPlayer());

	
	for (int i = 0; i < positions.size(); ++i)
	{
		IntPair* pair = undoAction->add_positon();
		pair->set_first(positions[i].first);
		pair->set_second(positions[i].second);
	}

	std::string serialized_command;
	command->SerializeToString(&serialized_command);
	return serialized_command;
}

std::vector<int> MessageHelper::SplitStringToInts(const std::string& str)
{
	std::vector<int> result;
	std::istringstream iss(str);
	std::string temp;

	while (iss >> temp)
	{
		result.push_back(std::stoi(temp));
	}

	return result;
}
