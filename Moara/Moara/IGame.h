#pragma once

#include "EGameExceptions.h"
#include "EGameType.h"
#include "EState.h"
#include "EPlayer.h"
#include "IGameListener.h"
#include "IPiece.h"
#include "IStrategy.h"
#include "Using.h"

#include <memory>
#include <vector>
#include <string>
#include <chrono>

using IGamePtr = std::shared_ptr<class IGame>;
using BoardMatrix = std::vector<std::vector<IPiecePtr>>;

/// <summary>
/// Interface representing a game.
/// </summary>
class IGame
{
public:
	/// <summary>
	/// Virtual destructor for the IGame interface.
	/// </summary>
	virtual ~IGame() = default;

	/// <summary>
	/// Produces an instance of the game.
	/// </summary>
	/// <param name="vsComputer">Indicates whether the game is against the computer.</param>
	/// <returns>A pointer to the created game instance.</returns>
	static IGamePtr Produce(bool vsComputer = false, bool testing = false);

	/// <summary>
	/// Places a piece on the board.
	/// </summary>
	/// <param name="placePos">The position to place the piece.</param>
	/// <returns>An exception if the operation fails.</returns>
	virtual EOperationResult PlacePiece(Pos placePos) = 0;

	/// <summary>
	/// Removes a piece from the board.
	/// </summary>
	/// <param name="removePos">The position to remove the piece from.</param>
	/// <returns>An exception if the operation fails.</returns>
	virtual EOperationResult RemovePiece(Pos removePos) = 0;

	/// <summary>
	/// Moves a piece on the board.
	/// </summary>
	/// <param name="initialPos">The initial position of the piece.</param>
	/// <param name="finalPos">The final position of the piece.</param>
	/// <returns>An exception if the operation fails.</returns>
	virtual EOperationResult MovePiece(Pos initialPos, Pos finalPos) = 0;

	/// <summary>
	/// Gets the current player.
	/// </summary>
	/// <returns>The current player.</returns>
	virtual EPlayer GetCurrentPlayer() const = 0;

	/// <summary>
	/// Gets the current state of the game.
	/// </summary>
	/// <returns>The current game state.</returns>
	virtual EState GetGameState() const = 0;

	/// <summary>
	/// Gets the current state of the board.
	/// </summary>
	/// <returns>A constant reference to the board matrix.</returns>
	virtual const BoardMatrix& GetBoard() const = 0;

	/// <summary>
	/// Determines if the game is being played against the computer.
	/// </summary>
	/// <returns>True if the game is against the computer, otherwise false.</returns>
	virtual const bool GetGameVsComputer() const = 0;

	/// <summary>
	/// Adds a listener to the game.
	/// </summary>
	/// <param name="listener">The listener to add.</param>
	virtual void AddListener(IGameListener* listener) = 0;

	/// <summary>
	/// Removes a listener from the game.
	/// </summary>
	/// <param name="listener">The listener to remove.</param>
	virtual void RemoveListener(IGameListener* listener) = 0;

	/// <summary>
	/// Save a game state to a file.
	/// </summary>
	/// <param name="filename">The name of the file to save the game state to.</param>
	/// <param name="historyfile">The name of the file to save the game history to.</param>
	/// <returns>True if the game state was saved successfully, false otherwise.</returns>
	virtual bool SaveGame(const std::string& filename, const std::string& historyfile) const = 0;

	/// <summary>
	/// Loads a game state from a file.
	/// </summary>
	/// <param name="filename">The name of the file to load the game state from.</param>
	/// <returns>True if the game state was loaded successfully, false otherwise.</returns>
	virtual bool LoadGame(const std::string& filename) = 0;

	/// <summary>
	/// Resets the game to its initial state.
	/// </summary>
	virtual void ResetGame() = 0;

	/// <summary>
	/// Undoes the last action performed in the game.
	/// </summary>
	/// <returns>True if the action was undone successfully, false otherwise.</returns>
	virtual bool Undo() = 0;

	/// <summary>
	/// Gets the possible positions to move a piece from a given position.
	/// </summary>
	/// <param name="position">The position to move the piece from.</param>
	/// <returns>A list of possible positions to move the piece to.</returns>
	virtual Positions WhereToMove(Pos position) const = 0;

	/// <summary>
	/// Gets the possible positions to capture a piece.
	/// </summary>
	/// <returns>A list of possible positions to capture a piece from.</returns>
	virtual Positions WhereToCapture() const = 0;

	/// <summary>
	/// Sets the strategy for the game.
	/// </summary>
	/// <param name="type">The type of strategy to set.</param>
	virtual void SetStrategy(EGameType type) = 0;

	/// <summary>
	/// Sets a custom strategy for the game.
	/// </summary>
	/// <param name="strategy">A pointer to the custom strategy to set.</param>
	virtual void SetStrategy(IStrategyPtr strategy) = 0;

	/// <summary>
	/// Starts the game timer.
	/// </summary>
	virtual void Start() = 0;

	/// <summary>
	/// Gets the time spent for an action. Checks whether the time has reached the limit of 10 seconds in whcih case it announces the winner.
	/// </summary>
	/// <returns>The duration of time spent by a player for an action in Miliseconds.</returns>
	virtual std::chrono::duration<long long, std::milli> GetTimeAction() = 0;

	/// <summary>
	/// Gets the time spent by a player.
	/// </summary>
	/// <param name="player">The player to get the time for.</param>
	/// <returns>The duration of time spent by the player in Miliseconds.</returns>
	virtual std::chrono::duration<long long, std::milli> GetTime(EPlayer player) = 0;
};

