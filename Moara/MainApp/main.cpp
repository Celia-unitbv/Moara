#include<iostream>
#include "logger.h"

#include "IGame.h"

using namespace std;

void PrintBoard(const std::vector<std::vector<IPiece*>>& board)
{
	std::cout << "Current Board:\n";
	for (auto& elem : board)
	{
		for (auto& e : elem)
			if (e)
			{
				if (!(int)(e->GetColor()))
					std::cout << "+ ";
				else std::cout << "# ";
			}
			else
				std::cout << "- ";
		std::cout << std::endl;
	}
}


int main()
{
//	LOG.log("Application started.Game State: Place. Current Player: Player 1", EMessageType::Info);
//
//	auto game = IGame::Produce(EGameType::Easy);
//	int x, y, row, col;
//	bool saved = 0;
//	do 
//	{
//		PrintBoard(game->GetBoard());
//		
// (game->GetGameState())
//		{
//		case EState::Player1Won:
//
//			std::cout << "Winner Player 1";
//			break;
//		case EState::Player2Won:
//
//			std::cout << "Winner Player 2";
//			break;
//
//		case EState::Place:
//
//			std::cout << "Player " << (int)game->GetCurrentPlayer() << " choose a position to place your piece: \n";
//			std::cin >> x >> y;
//			if (game->PlacePiece(x, y) == false)
//				{
//					std::cout << "Wrong position!\n";
//				}
//			break;
//
//		case EState::Move:
//
//			std::cout << "Player " << (int)game->GetCurrentPlayer() << " choose a piece to move: \n";
//			std::cin >> x >> y;
//			std::cout << "\nPlease choose where to move the piece:\n";
//			std::cin >> row >> col;
//			if (game->MovePiece(x, y, row, col) == false)
//			{
//				std::cout << "Wrong choice!\n";
//			}
//			break;
//
//		case EState::Capture:
//
//			std::cout << "You have made a line! Choose a piece from the opposite player to be removed: \n";
//			std::cin >> x >> y;
//			if (!game->RemovePiece(x, y))
//			{
//				std::cout << "Wrong Piece!\n";
//			}
//			if (saved == 0) {
//				game->SaveGame("save.txt"); saved = 1;
//			}
//			else game->LoadGame("save.txt");
//			break;
//		}
//
//	} while (game->GetGameState() != EState::Player2Won && game->GetGameState() != EState::Player1Won);
//
//	
//
//	
////std::cout << "Number of Piece instances: " << Piece::t << std::endl;
//	LOG.log("Application ended", EMessageType::Info);

	return 0;
}