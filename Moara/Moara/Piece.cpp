#include "Piece.h"
#include <math.h>
Piece::Piece() :m_colour(EColor::Red)
{
	//empty
}

Piece::Piece(const EColor& colour, int x, int y) : m_colour(colour), m_pozition({ x,y })
{
	//empty
}

void Piece::SetColour(int col)
{
	this->m_colour = (EColor)col;
}

void Piece::SetPozition(Pos position)
{
	this->m_pozition = position;
}

EColor Piece::GetColor() const
{
	return m_colour;
}

Pos Piece::GetPozition() const
{
	return m_pozition;
}

bool Piece::CheckPlace(Pos position) const
{
	auto [row, col] = position;
	int rowPiece = m_pozition.first;
	int colPiece = m_pozition.second;

	if (rowPiece == row && colPiece == col)
		return false;

	if (colPiece % 2 == 1)
	{
		if (colPiece == 7)
		{
			return (col == colPiece && std::abs(row - rowPiece) == 1) || (rowPiece == row && colPiece == col + 1) || (rowPiece == row && col == 0);
		}
		return (col == colPiece && std::abs(row - rowPiece) == 1) || (rowPiece == row && colPiece == col - 1) || (rowPiece == row && colPiece == col + 1);
	}

	if (colPiece == 0)
	{
		return ((rowPiece == row && colPiece == col - 1) || (rowPiece == row && col == 7));
	}

	return ((rowPiece == row && colPiece == col - 1) || (rowPiece == row && colPiece == col + 1));
}

