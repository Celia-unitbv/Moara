#include "Board.h"

#include "Logger.h"
static Logger& LOG = Logger::instance("app.log");

Board::Board(size_t rows, size_t cols)
	: m_board(rows, std::vector<IPiecePtr>(cols))
	, m_nr_Pieces(2)
	, m_nr_Lines(2)
	, m_total_pieces(18)
{

}

Board::~Board()
{
}

int Board::GetNrPieces(int cp) const
{
	return m_nr_Pieces[cp];
}

int Board::GetNrLines(int cp) const
{
	return m_nr_Lines[cp];
}

int Board::GetNrTotalPieces() const
{
	return m_total_pieces;
}

void Board::SetNrTotalPieces(int nrTotalPieces)
{
	m_total_pieces = nrTotalPieces;
}

void Board::SetNrPieces(int cp, int nrPieces)
{
	m_nr_Pieces[cp] = nrPieces;
}

void Board::SetNrLines(int cp, int nrLines)
{
	m_nr_Lines[cp] = nrLines;
}

void Board::SetPiece(int i, int j, IPiecePtr Piece)
{
	m_board[i][j] = Piece;
}

const BoardMatrix& Board::GetBoard() const
{
	return m_board;
}

Positions Board::PossibleCapture(int currentPlayer) const
{
	Positions moves;
	int nrPieces = m_nr_Pieces[(1 - currentPlayer)];
	if (nrPieces <= 3 || CheckAllLines(1-currentPlayer))
	{
		//daca adversar are 3 piese sau doar linii, selecteaza tot
		for (int i = 0; i < m_board.size(); i++)
			for (int j = 0; j < m_board[i].size(); j++)
			{
				if(m_board[i][j])
				  if ((int)m_board[i][j]->GetColor() == (1 - currentPlayer))
					 moves.push_back({ i,j });
			}
	
	}
	else 
	{
		for (int i = 0; i < m_board.size(); i++)
			for (int j = 0; j < m_board[i].size(); j++)
			{
				if (m_board[i][j])
					if ((int)m_board[i][j]->GetColor() == (1 - currentPlayer))
						if (!CheckLine({ i, j }, 1 - currentPlayer))
							moves.push_back({ i,j });
			}
	}
	return moves;
	
}

Positions Board::PossibleMove(int currentPlayer,Pos position) const
{
	Positions moves;
	auto [x, y] = position;
	int nrPieces = m_nr_Pieces[currentPlayer];
	if (nrPieces < 4)
	{
		for (int i = 0; i < m_board.size(); i++)
			for (int j = 0; j < m_board[i].size(); j++)
			{
				if (m_board[i][j]==nullptr)
					moves.push_back({ i,j });
			}
	}
	else
	{
		for (int i = 0; i < m_board.size(); i++)
			for (int j = 0; j < m_board[i].size(); j++)
			{
				if (!m_board[i][j] && m_board[x][y]->CheckPlace({ i,j }))
						 moves.push_back({ i,j });
			}
	}
	return moves;
}

bool Board::StillInitialize() const
{
	return (m_total_pieces >= 1);
}

EOperationResult Board::PlacePiece(IPiecePtr piece)
{
	int row = piece->GetPozition().first;
	int col = piece->GetPozition().second;

	if (row < m_board.size() && col < m_board[row].size())
	{
		if (!m_board[row][col])
		{
			LOG.log("Player placed piece!", EMessageType::Info);
			m_board[row][col] = piece;
			m_total_pieces--;
			m_nr_Pieces[(int)piece->GetColor()]++;
			if (CheckLine(piece->GetPozition(), (int)piece->GetColor()))
				m_nr_Lines[(int)piece->GetColor()]++;
			return EOperationResult::NoError;
		}
		
		LOG.log("Player tried and failed to place piece!", EMessageType::Warning);
		return EOperationResult::IsNotAValidPlace;
		

	}
	LOG.log("Player tried and failed to place piece!", EMessageType::Error);
	return EOperationResult::IndexOutOfBounds;
}

EOperationResult Board::RemovePiece(Pos removePos, int currentPlayerColor)
{
	auto [row, col] = removePos;
	if (row > m_board.size() && col > m_board[row].size()) return EOperationResult::IndexOutOfBounds;
	if(m_board[row][col])
	{
		if ((int)m_board[row][col]->GetColor() != currentPlayerColor)
		{
			Positions moves = PossibleCapture(currentPlayerColor);
			if (std::find(moves.begin(), moves.end(), std::make_pair(row, col)) != moves.end())
			{
				m_board[row][col] = nullptr;
				m_nr_Pieces[1 - currentPlayerColor]--;
				LOG.log("Player removed piece!", EMessageType::Info);
				return EOperationResult::NoError;
			}
			else
			{
				return EOperationResult::PieceIsPartOfALine;
			}
		}
		return EOperationResult::IsNotAValidPiece;
	}

	LOG.log("Player tried and failed to remove piece!", EMessageType::Warning);
	return EOperationResult::IsNotAValidPiece;
}

EOperationResult Board::MovePiece(Pos finalPos, IPiecePtr pieceToMove, int currentPlayer)
{
	int x = pieceToMove->GetPozition().first;
	int y = pieceToMove->GetPozition().second;
	auto [row, col] = finalPos;

	if ((int)pieceToMove->GetColor() != currentPlayer)
		return EOperationResult::IsNotAValidPiece;

	if (m_board[row][col] != nullptr)
	{
		LOG.log("Player tried and failed to move a piece!", EMessageType::Warning);
		return EOperationResult::IsNotAValidPlace;
	}
	
	Positions moves = PossibleMove(currentPlayer, { x,y });
	if (std::find(moves.begin(), moves.end(), std::make_pair(row, col)) != moves.end())
	{
		if (CheckLine(pieceToMove->GetPozition(), currentPlayer))
			m_nr_Lines[currentPlayer]--;

		pieceToMove->SetPozition(finalPos);
		PlacePiece(pieceToMove);

		m_total_pieces++;
		m_nr_Pieces[(int)currentPlayer]--;

		m_board[x][y] = nullptr;

		LOG.log("Player moved a piece!", EMessageType::Info);
		return EOperationResult::NoError;
	}
	
	LOG.log("Player tried and failed to move a piece!", EMessageType::Warning);
	return EOperationResult::IsNotAValidPlace;
}

void Board::UndoCapture(IPiecePtr piece, int cp)
{
	if (CheckLine(piece->GetPozition(), cp))
		m_nr_Lines[cp]++;
	m_board[piece->GetPozition().first][piece->GetPozition().second] = piece;
}

bool Board::IsSameColor(EColor piece1, EColor piece2, EColor piece3)
{
	return piece1 == piece2 && piece2 == piece3;
}

bool Board::CheckAllLines(int currentPlayerColor) const
{
	for (auto row : m_board)
	{
		for (auto node : row)
		{
			if (node && (int)node->GetColor() == currentPlayerColor && !CheckLine(node->GetPozition(), currentPlayerColor))
						return false;
		}
	}
	return true;
}

bool Board::Blocaj(int currentPlayer)
{
	for (int i = 0; i < m_board.size(); i++)
		for (int j = 0; j < m_board[i].size(); j++)
		{
			if(m_board[i][j] && (int)m_board[i][j]->GetColor() == (currentPlayer) && PossibleMove(currentPlayer, { i, j }).size() > 0)
						return false;
		}
	return true;
}

bool Board::CheckLine(Pos position, int color) const
{
	auto [row, col] = position;
	if (col % 2 == 1)//caz impar
	{
		if (m_board[row][(col + 1) % 8] && m_board[row][col - 1])
		{
			if (IsSameColor((EColor)color, m_board[row][(col + 1) % 8]->GetColor(), m_board[row][col - 1]->GetColor()))
			{
				LOG.log("Player made a line!", EMessageType::Info);
				return true;
			}
		}

		if ((row + 1) % 3 == 1 && m_board[row + 1][col] && m_board[row + 2][col])
		{
			if (IsSameColor((EColor)color, m_board[row + 1][col]->GetColor(), m_board[row + 2][col]->GetColor()))
			{
				LOG.log("Player made a line!", EMessageType::Info);
				return true;

			}
		}

		if ((row + 1) % 3 == 2 && m_board[row - 1][col] && m_board[row][col] && m_board[row + 1][col])
		{
			if (IsSameColor((EColor)color, m_board[row + 1][col]->GetColor(), m_board[row - 1][col]->GetColor()))
			{
				LOG.log("Player made a line!", EMessageType::Info);
				return true;
			}
		}
		else if ((row + 1) % 3 == 0 && m_board[row - 1][col] && m_board[row][col] && m_board[row - 2][col])
		{
			if (IsSameColor((EColor)color, m_board[row - 1][col]->GetColor(), m_board[row - 2][col]->GetColor()))
			{
				LOG.log("Player made a line!", EMessageType::Info);
				return true;
			}
		}
		LOG.log("Player didn't make a line!", EMessageType::Warning);
		return false;
	}
	if (m_board[row][col + 1] && m_board[row][(col + 2) % 8])
		if (IsSameColor((EColor)color, m_board[row][col + 1]->GetColor(), m_board[row][(col + 2) % 8]->GetColor()))
		{
			LOG.log("Player made a line!", EMessageType::Info);
			return true;
		}
	if (m_board[row][(col - 1 + 8) % 8] && m_board[row][(col - 2 + 8) % 8])
	{
		if (IsSameColor((EColor)color, m_board[row][(col - 1 + 8) % 8]->GetColor(), m_board[row][(col - 2 + 8) % 8]->GetColor()))
		{
			LOG.log("Player made a line!", EMessageType::Info);
			return true;
		}
	}

	LOG.log("Player didn't make a line!", EMessageType::Warning);
	return false;
}

void Board::UndoPlace(Pos placePos, int currentPlayer)
{
	auto [row, col] = placePos;

	if (CheckLine(placePos, currentPlayer))
		m_nr_Lines[currentPlayer]--;

	m_board[row][col] = nullptr;

	m_nr_Pieces[currentPlayer]--;
	m_total_pieces++;
}

void Board::UndoMove(Pos fromPos, IPiecePtr piece, int currentPlayer)
{
	auto [rowMove, colMove] = fromPos;
	if (CheckLine(piece->GetPozition(), currentPlayer))
		m_nr_Lines[currentPlayer]--;
	m_board[piece->GetPozition().first][piece->GetPozition().second] = nullptr;
	piece->SetPozition(fromPos);
	m_board[rowMove][colMove] = piece;
}

IPiecePtr Board::GetPiece(Pos position) const
{
	auto [x, y] = position;
	if(x<3 && y<8 && x>=0 && y>=0)
		return m_board[x][y];
	return nullptr;
}