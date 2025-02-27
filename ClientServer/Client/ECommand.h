#pragma once

enum class ECommand
{
	State,
	Place,
	Move,
	Capture,
	WhereToMove,
	WhereToCapture,
	Reset,
	Undo,
	ProxyPlayer,
	Time,
	StartGame,
	Startegy,
	Win,
	OpponentDisconnect,
	ServerDisconnect,
	StartLobby,
	GameAlreadyExists,
	Error
};