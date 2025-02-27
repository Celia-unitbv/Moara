#include "Game.h"

#include "Logger.h"
#include "EasyStrategy.h"
#include "MediumStartegy.h"
#include "Piece.h"

#include <chrono>

#include <fstream>

using namespace std::chrono;

static Logger& LOG = Logger::instance("app.log");

Game::Game(bool vsComputer/* = false*/, bool testing/* = false*/)
	: m_Board(3, 8)
	, m_GameState(EState::Place)
	, m_currentPlayer(EPlayer::Player1)
	, m_vsComputer(vsComputer)
	, m_timeAction(10 * 1000)
	, m_firstMove(false)
	, m_testing(testing)
{
	SetStrategy(EGameType::Easy);
}

EState Game::GetGameState() const
{
	return m_GameState;
}

const BoardMatrix& Game::GetBoard() const
{
	return m_Board.GetBoard();
}

const bool Game::GetGameVsComputer() const
{
	return m_vsComputer;
}

bool Game::SaveGame(const std::string& filename, const std::string& historyfile) const
{
	std::ofstream file(filename);
	if (!file.is_open())
	{
		return false;
	}

	file << static_cast<int>(m_GameState) << "\n";
	file << static_cast<int>(m_currentPlayer) << "\n";
	file << m_Board.GetNrPieces(0) << " " << m_Board.GetNrPieces(1) << "\n";
	file << m_Board.GetNrLines(0) << " " << m_Board.GetNrLines(1) << "\n";
	file << m_Board.GetNrTotalPieces() << "\n";

	BoardMatrix board = m_Board.GetBoard();
	for (const auto& row : board)
	{
		for (const auto& piece : row)
		{
			if (piece != nullptr)
			{
				file << static_cast<int>(piece->GetColor()) << " ";
			}
			else
			{
				file << "-1 ";
			}
		}
		file << "\n";
	}

	file.close();

	std::ofstream outFile(historyfile);
	if (!outFile.is_open())
	{
		std::cerr << "Failed to open file for writing: " << historyfile << std::endl;
		return false;
	}

	for (const auto& entry : m_gameHistory)
	{
		outFile << static_cast<int>(entry.m_state) << std::endl;

		for (const auto& piece : entry.m_piece)
		{
			outFile << piece.first << " " << piece.second << std::endl;
		}

		outFile << "-----" << std::endl;
	}

	outFile.close();

	NotifyListenersOnSave();
	return true;
}

bool Game::LoadGame(const std::string& filename) {
	ResetGame();
	std::ifstream file(filename);
	if (!file.is_open())
	{
		return false;
	}

	int gameState;
	file >> gameState;
	m_GameState = static_cast<EState>(gameState);

	int currentPlayer;
	file >> currentPlayer;
	m_currentPlayer = static_cast<EPlayer>(currentPlayer);

	int nrPieces0, nrPieces1;
	file >> nrPieces0 >> nrPieces1;

	int nrLines0, nrLines1,nrTotal;
	file >> nrLines0 >> nrLines1;

	file >> nrTotal;
	size_t rows, cols;
	rows = 3;
	cols = 8;

	std::vector<std::vector<int>> BoardColors;

	for (size_t i = 0; i < rows; ++i)
	{
		std::vector<int> Boardaux;
		for (size_t j = 0; j < cols; ++j)
		{
			int color;
			file >> color;
			Boardaux.push_back(color);
		}
		BoardColors.push_back(Boardaux);
	}
	LoadBoard(BoardColors);

	m_Board.SetNrPieces(0, nrPieces0);
	m_Board.SetNrPieces(1, nrPieces1);
	m_Board.SetNrLines(0, nrLines0);
	m_Board.SetNrLines(1, nrLines1);
	m_Board.SetNrTotalPieces(nrTotal);

	file.close();
	NotifyListenersOnLoad();
	return true;
}

bool Game::Undo()
{
	if (m_gameHistory.empty()) 
		return false;
	
	if (m_GameState == EState::Player1Won || m_GameState == EState::Player2Won) 
		return false;
	
	GameHistory action = m_gameHistory.back();
	m_gameHistory.pop_back();

	if (action.m_state == EState::Place)//daca a fost place
	{
		if (m_GameState != EState::Capture)
			 SwitchPlayers();
		m_GameState = EState::Place;
		m_Board.UndoPlace(action.m_piece[0], (int)m_currentPlayer);
	}

	if (action.m_state == EState::Move)
	{
		if (m_GameState != EState::Capture)

			  SwitchPlayers();
		m_Board.UndoMove(action.m_piece[0], m_Board.GetPiece(action.m_piece[1]), (int)m_currentPlayer);
		m_GameState = EState::Move;
	}

	if (action.m_state == EState::Capture)
	{
		IPiecePtr removedPiece = CreatePiece(action.m_piece[0]);//culoare adversar, de la cine am luat
		m_Board.UndoCapture(removedPiece, (int)m_currentPlayer);
		m_GameState = EState::Capture;
		  SwitchPlayers();
	}

	NotifyListenersOnUndo(action.m_piece);

	return true;
}

void Game::ResetGame()
{
	m_GameState = EState::Place;
	m_currentPlayer = EPlayer::Player1;

	m_Board = Board(3, 8);
	
	m_timer.Reset();
	NotifyListenersOnReset();
}

Positions Game::WhereToMove(Pos position) const
{
	return m_Board.PossibleMove((int)m_currentPlayer, position);
}

Positions Game::WhereToCapture() const
{
	return m_Board.PossibleCapture((int)m_currentPlayer);
}



void Game::AddListener(IGameListener* listener)
{
	LOG.log("Listener added to observer Game!", EMessageType::Info);

	m_Listeners.emplace_back(listener);
}

void Game::RemoveListener(IGameListener* listener)
{
	auto func = [listener](IGameListener* el)
	{
		return el == listener;
	};

	m_Listeners.erase(std::remove_if(m_Listeners.begin(), m_Listeners.end(), func));
	LOG.log("Listener removed from observer Game!", EMessageType::Info);

}

IGamePtr IGame::Produce(bool vsComputer, bool testing)
{
	LOG.log("Game created based on difficulty!", EMessageType::Info);
	return std::make_shared<Game>(vsComputer,testing);
}

void Game::AddAction(EState state, const std::vector < std::pair<int,int>>&pieces)
{
	GameHistory action;
	action.m_state = state;
	action.m_piece = pieces;
	m_gameHistory.push_back(action);
}

void Game::NotifyListenersPlacePiece(Pos placePos) const
{
	for (auto it : m_Listeners)
	{
		it->OnPlacePiece(placePos, (int)m_currentPlayer);
	}
}

void Game::NotifyListenersRemovePiece(Pos removePos) const
{
	for (auto it : m_Listeners)
	{
		it->OnRemovePiece(removePos);
	}
}

void Game::NotifyListenersOnUndo(Positions pieces) const
{
	for (auto it : m_Listeners)
	{
		it->OnUndo(pieces);
	}
}

void Game::NotifyListenersOnLoad() const
{
	for (auto it : m_Listeners)
	{
		it->OnLoad();
	}
}

void Game::NotifyListenersOnReset() const
{
	for (auto it : m_Listeners)
	{
		it->OnReset();
	}
}

void Game::NotifyListenersOnSave() const
{
	for (auto it : m_Listeners)
	{
		it->OnSave();
	}
}

void Game::NotifyListenersOnWin() const
{
	for (auto it : m_Listeners)
	{
		it->OnWin();
	}
}

void Game::NotifyListenersOnError(int result) const
{
	for (auto it : m_Listeners)
	{
		it->OnError(result);
	}
}

void Game::NotifyListenersMovePiece(Pos initialPos, Pos finalPos) const
{
	for (auto it : m_Listeners)
	{
		it->OnMovePiece(initialPos, finalPos, (int)m_currentPlayer);
	}
}

bool Game::CheckLine(Pos position) const
{
	return m_Board.CheckLine(position, (int)m_currentPlayer);
}

EOperationResult Game::PlacePiece(Pos placePos)
{
	EOperationResult result = m_Board.PlacePiece(CreatePiece(placePos));
	
	if (result == EOperationResult::NoError)
	{
		AddAction(EState::Place, { placePos });
		if (CheckLine(placePos))
		{
			LOG.log("Game State: Capture!", EMessageType::Info);
			m_GameState = EState::Capture;
		}
		else
		{
			if (!m_Board.StillInitialize())
			{
				m_GameState = EState::Move;
				LOG.log("Game State: Move!", EMessageType::Info);
			}
		 if (m_Board.GetNrTotalPieces() == 0)
			{
				if (m_Board.Blocaj(1-(int)m_currentPlayer))
				{
					m_GameState = m_currentPlayer == EPlayer::Player1 ? EState::Player1Won : EState::Player2Won;

					m_currentPlayer = EPlayer(1 - (int)m_currentPlayer);
					NotifyListenersOnWin();
					return EOperationResult::NoError;
				}
			}
			NotifyListenersPlacePiece(placePos);
			SwitchPlayers();
			
			return EOperationResult::NoError;
		}
		NotifyListenersPlacePiece(placePos);
		return EOperationResult::NoError;
	}
	NotifyListenersOnError((int)result);
	return result;
}

EOperationResult Game::RemovePiece(Pos RemovePos)
{
	EOperationResult result = m_Board.RemovePiece(RemovePos, (int)m_currentPlayer);

	if (result != EOperationResult::NoError)
	{
		NotifyListenersOnError((int)result);
		return result;
	}

	AddAction(EState::Capture, { RemovePos });

	if (m_Board.StillInitialize())
	{
		m_GameState = EState::Place;
		LOG.log("Game State: Place!", EMessageType::Info);
	}
	else
	{
		if (m_Board.Blocaj(1 - (int)m_currentPlayer))
		{
			m_currentPlayer = EPlayer(1 - (int)m_currentPlayer);
			m_GameState = m_currentPlayer == EPlayer::Player1 ? EState::Player1Won : EState::Player2Won;
			NotifyListenersOnWin();
			return EOperationResult::NoError;
		}
		m_GameState = EState::Move;
		LOG.log("Game State: Move!", EMessageType::Info);

		if (m_Board.GetNrPieces(!(int)m_currentPlayer) < 3)
		{
			m_currentPlayer = EPlayer(1 - (int)m_currentPlayer);
			m_GameState = m_currentPlayer == EPlayer::Player1 ? EState::Player2Won : EState::Player1Won;
			NotifyListenersOnWin();
			return EOperationResult::NoError;
		}
		
		LOG.log("The winner is found!", EMessageType::Info);
	}
	NotifyListenersRemovePiece(RemovePos);
	  SwitchPlayers();

	return EOperationResult::NoError;
}

EOperationResult Game::MovePiece(Pos initialPos, Pos finalPos)
{
	if (IPiecePtr p = m_Board.GetPiece(initialPos))
	{
		auto result = m_Board.MovePiece(finalPos, p, (int)m_currentPlayer);
		if (result != EOperationResult::NoError)
		{
			NotifyListenersOnError((int)result);
			return result;
		}
		AddAction(EState::Move, { initialPos,finalPos });
		NotifyListenersMovePiece(initialPos, finalPos);

		if (!CheckLine(finalPos))
		{
			if (m_Board.Blocaj(1 - (int) m_currentPlayer))
			{
				m_GameState = m_currentPlayer == EPlayer::Player1 ? EState::Player1Won : EState::Player2Won;
				NotifyListenersOnWin();
				return EOperationResult::NoError;
			}
			SwitchPlayers();
		}
		else
		{
			m_GameState = EState::Capture;
			LOG.log("Game State: Capture!", EMessageType::Info);
		}
		return EOperationResult::NoError;
	}
	NotifyListenersOnError((int)EOperationResult::IsNotAValidPiece);
	return EOperationResult::IsNotAValidPiece;
}

EOperationResult Game::MakeComputerAction()
{
	EOperationResult result = EOperationResult::NoError;
	switch (m_GameState)
	{
	case EState::Place:
	{
		Pos placePos = m_strategy->PlacePiece(m_currentPlayer);
		result = PlacePiece(placePos);
		if(m_GameState==EState::Capture)
		{
			Pos removePos = m_strategy->CapturePiece(m_currentPlayer);
		result = RemovePiece(removePos);
		}
	}
	break;
	case EState::Move:
	{
		std::pair<Pos, Pos> position = m_strategy->MovePiece(m_currentPlayer);
		result = MovePiece(position.first, position.second);
		if (m_GameState == EState::Capture)
		{
			Pos removePos = m_strategy->CapturePiece(m_currentPlayer);
			result = RemovePiece(removePos);
		}
	}
	break;

	};

	return result;
}

EPlayer Game::GetCurrentPlayer() const
{
	return m_currentPlayer;
}

IPiecePtr Game::CreatePiece(Pos PieceCoordinates) const
{
	return std::make_shared<Piece>((EColor)m_currentPlayer, PieceCoordinates.first, PieceCoordinates.second);
}

IPiecePtr Game::CreatePieceColor(int x, int y, int color)
{
	return std::make_shared<Piece>((EColor)color, x, y);
}

void Game::SetStrategy(IStrategyPtr strategy)
{
	m_strategy = strategy;
}

void Game::LoadBoard(std::vector<std::vector<int>> BoardColors)
{
	for (int i = 0; i < BoardColors.size(); ++i)
	{
		for (int j = 0; j < BoardColors[i].size(); ++j)
		{
			if (BoardColors[i][j] != -1)
			{
				m_Board.SetPiece(i, j, CreatePieceColor(i, j, BoardColors[i][j]));
			}
		}
	}

}

void Game::SwitchPlayers()
{
	if (!m_vsComputer)
	{
		m_currentPlayer = EPlayer(1 - (int)m_currentPlayer);

		m_timer.SwitchPlayers();

		LOG.log("Player changed!", EMessageType::Info);
	}
	else {
		if (m_currentPlayer == EPlayer::Player1|| m_GameState == EState::Capture)
		{
			m_currentPlayer = EPlayer(1 - (int)m_currentPlayer);
			m_timer.SwitchPlayers();
			if (m_testing) MakeComputerAction();
			else {
				int index = m_randomGenerator.getRandomNumber(5);
				m_timer.SetAction(Miliseconds(index * 1000), [&]() {
					MakeComputerAction();
					});
			}
		}
		else
		{
			m_currentPlayer = EPlayer(1 - (int)m_currentPlayer);
			m_timer.OppositePlayer();
		}
	}
	
}

void Game:: SetStrategy(EGameType type)
{
	switch (type)
	{
	case EGameType::Easy:
		m_strategy = std::make_shared<EasyStrategy>(m_Board);
		break;
	case EGameType::Medium:
		m_strategy = std::make_shared<MediumStrategy>(m_Board);
		break;
	default:
		break;
	}
}

Miliseconds Game::GetTime(EPlayer player)
{
	return m_timer.GetPlayerTime(player);
}

void Game::Start()
{
	if (!m_timer.IsRunning())
	{
		m_timer.Start();
	}
}

void Game::Stop()
{
	if (m_currentPlayer == EPlayer::Player1)
		m_GameState = EState::Player2Won;
	else
		m_GameState = EState::Player1Won;
	
	m_timer.Stop();
}

Miliseconds Game::GetTimeAction()
{
	if (m_timer.GetPlayerTime(m_currentPlayer) <= Miliseconds(0))
	{
		m_GameState = m_currentPlayer == EPlayer::Player1 ? EState::Player2Won : EState::Player1Won;
		NotifyListenersOnWin();
		Stop();
	}
	Miliseconds time= m_timer.GetTimeAction(m_currentPlayer);
	if (time > m_timeAction)
	{
		m_GameState = m_currentPlayer == EPlayer::Player1 ? EState::Player2Won : EState::Player1Won;
		NotifyListenersOnWin();
		Stop();
	}
	
	return time;
}

