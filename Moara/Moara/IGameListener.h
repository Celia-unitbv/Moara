#pragma once

#include "EState.h"
#include "Using.h"

#include <vector>

/// <summary>
/// Interface for receiving notifications about game events.
/// </summary>
class IGameListener
{
public:
	/// <summary>
	/// Called when a piece is placed on the board.
	/// </summary>
	/// <param name="placePos">The position where the piece is placed.</param>
	/// <param name="color">The color of the piece placed.</param>
	virtual void OnPlacePiece(Pos placePos, int color) = 0;

	/// <summary>
	/// Called when a piece is removed from the board.
	/// </summary>
	/// <param name="removePos">The position where the piece is removed from.</param>
	virtual void OnRemovePiece(Pos removePos) = 0;

	/// <summary>
	/// Called when a piece is moved on the board.
	/// </summary>
	/// <param name="initialPos">The initial position of the piece.</param>
	/// <param name="finalPos">The final position of the piece.</param>
	/// <param name="currentPlayer">The current player making the move.</param>
	virtual void OnMovePiece(Pos initialPos, Pos finalPos, int currentPlayer) = 0;

	/// <summary>
	/// Called when the game is loaded from file.
	/// </summary>
	virtual void OnLoad() = 0;

	/// <summary>
	/// Called when the game is reset.
	/// </summary>
	virtual void OnReset() = 0;

	/// <summary>
	/// Called when the game is saved on file.
	/// </summary>
	virtual void OnSave() = 0;

	/// <summary>
	/// Called when an undo action is performed.
	/// </summary>
	/// <param name="positions">The positions affected by the undo action.</param>
	virtual void OnUndo(const Positions& positions) = 0;

	/// <summary>
	/// Called when a player won.
	/// </summary>
	virtual void OnWin() = 0;

	/// <summary>
	/// Virtual destructor for the IGameListener interface.
	/// </summary>
	virtual ~IGameListener() = default;

	virtual  void OnError(int result) = 0;
};

using GameListenerList = std::vector<IGameListener*>;