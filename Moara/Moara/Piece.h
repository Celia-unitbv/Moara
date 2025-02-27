#pragma once

#include "EColor.h"
#include "IPiece.h"

#include <iostream>

class Piece:public IPiece
{
public:
	Piece();
	Piece(const EColor& colour, int row, int col);

	//IPiece methods
	EColor GetColor() const override;
	bool CheckPlace(Pos position) const override;
	Pos GetPozition() const override;
	void SetPozition(Pos position) override;

	//setter
	void SetColour(int col);
	
	~Piece() { } 

private:
	EColor m_colour;
	Pos m_pozition;

};

