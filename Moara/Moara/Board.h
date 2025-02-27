#pragma once

#include "EGameExceptions.h"
#include "IPiece.h"
#include "Using.h"

#include <memory>
#include <vector>

using BoardMatrix = std::vector<std::vector<IPiecePtr>>;

class Board
{
public:
	Board(size_t rows, size_t cols);
	~Board();

	//Getter
	int GetNrPieces(int currentPlayer) const;
	int GetNrLines(int currentPlayer) const;
	int GetNrTotalPieces() const;
	const BoardMatrix& GetBoard() const;
	IPiecePtr GetPiece(Pos position) const;

	//Setter
	void SetNrPieces(int currentPlayer, int nrPieces);
	void SetNrLines(int currentPlayer, int nrLines);
	void SetNrTotalPieces(int nrTotalPieces);
	void SetPiece(int i, int j, IPiecePtr Piece);
	
	//Piece methods
	EOperationResult PlacePiece(IPiecePtr piece);
	EOperationResult RemovePiece(Pos removePos, int currentPlayerColor);
	EOperationResult MovePiece(Pos finalPos, IPiecePtr pieceToMove,int currentPlayer);

	//Undo methods
	void UndoPlace(Pos placePos, int currentPlayer);
	void UndoMove(Pos fromPos, IPiecePtr piece, int currentPlayer);
	void UndoCapture(IPiecePtr piece, int currentPlayer);

	bool StillInitialize() const;

	bool Blocaj(int currentPlayer);

	bool CheckLine(Pos position, int color) const;

	//Possible Actions
	Positions PossibleCapture(int currentPlayer)const;
	Positions PossibleMove(int currentPlayer, Pos position)const;

private:
	static bool IsSameColor(EColor piece1, EColor piece2, EColor piece3); 
	bool CheckAllLines(int currentPlayerColor)const;
	
private:

	int m_total_pieces;
	BoardMatrix m_board;
	std::vector<int> m_nr_Pieces;
	std::vector<int> m_nr_Lines;
};

