#include "MessageHelper.h"
#include <sstream>

MessageHelperClient::MessageHelperClient()
{
	command = new Command();
}

const std::string MessageHelperClient::GetMessageForWhereToMove(const Pos nod) const
{
	command->set_command(Ecommand::WhereToMove);

	WhereToMovePiece* place_data = command->mutable_wheretomovepiece();
	place_data->set_x(nod.first);
	place_data->set_y(nod.second);

	std::string serialized_command;
	command->SerializeToString(&serialized_command);
	command->Clear();
	return serialized_command;

}


const std::string MessageHelperClient::GetMessageForPlacePiece(const Pos position) const
{
	command->set_command(Ecommand::Place);

	PlacePiece* place_data = command->mutable_place();
	place_data->set_x(position.first);
	place_data->set_y(position.second);

	std::string serialized_command;
	command->SerializeToString(&serialized_command);
	command->Clear();
	return serialized_command;
}

const std::string MessageHelperClient::GetMessageForServer(const std::string& gameName, const std::string& message) const
{
	std::stringstream ss;
	ss << "message," << gameName << "," << message;
	return ss.str();
}

const std::string MessageHelperClient::GetMessageForCapturePiece(const Pos position) const
{
	command->set_command(Ecommand::Capture);

	CapturePiece* place_data = command->mutable_capture();
	place_data->set_x(position.first);
	place_data->set_y(position.second);

	std::string serialized_command;
	command->SerializeToString(&serialized_command);
	command->Clear();
	return serialized_command;
}

const std::string MessageHelperClient::GetMessageForMovePiece(const Pos initialPosition, const Pos finalPosition) const
{
	command->set_command(Ecommand::Move);

	MovePiece* place_data = command->mutable_move();
	place_data->set_from_x(initialPosition.first);
	place_data->set_from_y(initialPosition.second);
	place_data->set_to_x(finalPosition.first);
	place_data->set_to_y(finalPosition.second);
	
	std::string serialized_command;
	command->SerializeToString(&serialized_command);
	command->Clear();
	return serialized_command;

}

const std::string MessageHelperClient::GetMessageForSave() const
{
	return "";
}


const std::string MessageHelperClient::GetMessageForReset() const
{
	command->set_command(Ecommand::Reset);

	std::string serialized_command;
	command->SerializeToString(&serialized_command);
	command->Clear();
	return serialized_command;
}

const std::string MessageHelperClient::GetMessageForStart() const
{
	command->set_command(Ecommand::StartGame);

	std::string serialized_command;
	command->SerializeToString(&serialized_command);
	command->Clear();
	return serialized_command;
	
}

const std::string MessageHelperClient::GetMessageForTime() const
{
	command->set_command(Ecommand::Time);

	std::string serialized_command;
	command->SerializeToString(&serialized_command);
	command->Clear();
	return serialized_command;

}

const std::string MessageHelperClient::GetMessageForProxyPlayer() const
{
	command->set_command(Ecommand::ProxyPlayer);

	std::string serialized_command;
	command->SerializeToString(&serialized_command);
	command->Clear();
	return serialized_command;

}

const std::string MessageHelperClient::GetMessageForLoad() const
{
	return "";
}

const std::string MessageHelperClient::GetMessageForUndo() const
{
	command->set_command(Ecommand::Undo);

	std::string serialized_command;
	command->SerializeToString(&serialized_command);
	command->Clear();
	return serialized_command;
}

const std::string MessageHelperClient::GetMessageForStrategy(int strategy) const
{
	command->set_command(Ecommand::Strategy);

	SetStrategy* strg = command->mutable_setstrategy();
	strg->set_x(strategy);

	std::string serialized_command;
	command->SerializeToString(&serialized_command);
	command->Clear();
	return serialized_command;
		
}

const std::string MessageHelperClient::GetMessageForGameState() const
{
	command->set_command(Ecommand::State);

	std::string serialized_command;
	command->SerializeToString(&serialized_command);
	command->Clear();
	return serialized_command;
}
