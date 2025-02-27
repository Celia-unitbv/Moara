#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

#include "Game.h"
#include "IGameListener.h"
#include "CustomStrategy.h"
#include "Logger.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Eq;

class MockGameListener : public IGameListener {
public:
    MOCK_METHOD(void, OnPlacePiece, (Pos placePos, int color), (override));
    MOCK_METHOD(void, OnMovePiece, (Pos initialPos, Pos finalPos, int currentPlayer), (override));
    MOCK_METHOD(void, OnRemovePiece, (Pos removePos), (override));
    MOCK_METHOD(void, OnLoad, (), (override));
    MOCK_METHOD(void, OnReset, (), (override));
    MOCK_METHOD(void, OnSave, (), (override));
    MOCK_METHOD(void, OnWin, (), (override));
    MOCK_METHOD(void, OnUndo,(const Positions& positions), (override));
	
};

class GameTest : public ::testing::Test
{
protected:
	Game game;
	BoardMatrix board;
	MockGameListener listener;

	void SetUp() override
	{
		game.AddListener(&listener);
		game.SetStrategy(EGameType::Easy);
	}
	void sayHi()
	{
		return;
	}
};

class GameTestComputer : public ::testing::Test
{
protected:
	IGamePtr game;
	BoardMatrix board;
	MockGameListener listener;

	void SetUp() override
	{
		game = IGame::Produce(true,true);  
		game->AddListener(&listener);
		game->SetStrategy(EGameType::Easy);
	}

	void sayHi()
	{
		return;
	}
};

TEST_F(GameTest, Place1PieceNotifiesListeners) {
	EXPECT_CALL(listener, OnPlacePiece(Pos{ 1, 1 }, 0)).Times(1);
	
	EOperationResult result = game.PlacePiece({ 1, 1 });
    EXPECT_EQ(result,EOperationResult::NoError);
}

TEST_F(GameTest, Remove2PiecesNotifiesListener) {//WARNING

	EXPECT_CALL(listener, OnPlacePiece(Pos{ 1, 1 },0)).Times(1);
	EXPECT_CALL(listener, OnPlacePiece(Pos{ 0, 0 }, 1)).Times(1);

	EXPECT_CALL(listener, OnRemovePiece((Pos{ 0, 0 }))).Times(1);
	EXPECT_CALL(listener, OnRemovePiece(Pos{ 1, 1 })).Times(1);

	game.PlacePiece({ 1, 1 });
	game.PlacePiece({ 0, 0 });

 
	EOperationResult result = game.RemovePiece({ 0, 0 });
	EOperationResult result1 = game.RemovePiece({ 1, 1 });

    ASSERT_TRUE(result==EOperationResult::NoError && result1==EOperationResult::NoError);
}

TEST_F(GameTest, RemovePieceNotifiesListeners) {
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);
	EXPECT_CALL(listener, OnRemovePiece(Pos{ 0, 0 })).Times(1);

	game.LoadGame("GameCapture.txt");

	EOperationResult result = game.RemovePiece({ 0, 0 });

	EXPECT_EQ(result, EOperationResult::NoError);
}

TEST_F(GameTest, MovePieceNotifiesListeners) {
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);
	EXPECT_CALL(listener, OnMovePiece(Pos{ 2, 5 }, Pos{ 1, 5 },1)).Times(1);

	game.LoadGame("GameBlocaj.txt");

	EXPECT_EQ(game.MovePiece({ 2, 5 }, { 1, 5 }), EOperationResult::NoError);
}

TEST_F(GameTest, Move2PiecesNotifiesListeners)
{
	EXPECT_CALL(listener, OnMovePiece(Pos{ 1, 1 }, Pos{ 0, 1 }, 0)).Times(1);

	EXPECT_CALL(listener, OnMovePiece(Pos{0, 0}, Pos{0, 7}, 1)).Times(1);

	game.PlacePiece({ 1, 1 });
	game.PlacePiece({ 0, 0 });

	EOperationResult result = game.MovePiece({ 1, 1 }, { 0, 1 });
	EOperationResult result1 = game.MovePiece({ 0, 0 }, { 0, 7 });

	EXPECT_EQ(result, EOperationResult::NoError);
	EXPECT_EQ(result1, EOperationResult::NoError);
}

TEST_F(GameTest, RemovePiece_RemovePieceInOnly1Line)
{
    auto board = game.GetBoard();
    game.PlacePiece({1, 1});//r
    game.PlacePiece({0, 0});//n
    game.PlacePiece({1, 2});//r
    game.PlacePiece({0,1});//n
    game.PlacePiece({1,3});//r

    EXPECT_EQ(game.RemovePiece(Pos{1, 1}), EOperationResult::NoError);//n
}

TEST_F(GameTest, SaveGame) {
	EXPECT_CALL(listener, OnSave()).Times(1);
	EXPECT_CALL(listener, OnPlacePiece(Pos{ 0,0 },0)).Times(1);

	game.PlacePiece({ 0,0 });
	bool result = game.SaveGame("test_save.txt","SaveHistory.txt");

	ASSERT_TRUE(result);
}

TEST_F(GameTest, SaveGame_Fail) {
	bool result = game.SaveGame("/root/test_fail.txt", "/root/history_fail.txt");

	ASSERT_FALSE(result);
}

TEST_F(GameTest, SaveGame_Fail_History) {
	bool result = game.SaveGame("test.txt", "/root/history_fail.txt");

	ASSERT_FALSE(result);
}


TEST_F(GameTest, LoadGame) {
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);

	bool result = game.LoadGame("test_save.txt");

	EXPECT_TRUE(result);
}

TEST_F(GameTest, GameGetState) {
    EXPECT_EQ(game.GetGameState(), EState::Place);
}

TEST_F(GameTest, GameGetCurrentPlayer)
{
	EXPECT_EQ(game.GetCurrentPlayer(), EPlayer::Player1);
}

TEST_F(GameTest, GameSwitchPlayers)
{
	EXPECT_CALL(listener, OnPlacePiece(Pos{ 1, 1 },0)).Times(1);
	EOperationResult result = game.PlacePiece({ 1, 1 });

    EXPECT_EQ(result, EOperationResult::NoError);

	EXPECT_EQ(game.GetCurrentPlayer(), EPlayer::Player2);
}

TEST_F(GameTest, GetBoard) {
	auto board = game.GetBoard();

	EXPECT_EQ(board.size(), 3);
    for (int i = 0; i < board.size(); i++)
    {
        EXPECT_EQ(board[i].size(), 8);

    }
}

TEST_F(GameTest, State_StateCapture)
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);
	EXPECT_CALL(listener, OnPlacePiece(Pos{ 2, 1 },0)).Times(1);

	game.LoadGame("GameAlmostLine.txt");
    
	game.PlacePiece({ 2, 1 });

    EXPECT_EQ(game.GetGameState(),EState::Capture);
}

TEST_F(GameTest, State_StatePlace)
{
	EXPECT_CALL(listener, OnPlacePiece(Pos{ 0, 1 }, 0)).Times(1);
	game.PlacePiece({ 0, 1 });//r

    EXPECT_EQ(game.GetGameState(), EState::Place);//n
}

TEST_F(GameTest, State_StateMove)
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);
	EXPECT_CALL(listener, OnPlacePiece(Pos{ 0,6 },1)).Times(1);
	game.LoadGame("GamePlaceStateToMove.txt");

	game.PlacePiece({ 0, 6 });
	EXPECT_EQ(game.GetGameState(), EState::Move);//n
}


TEST_F(GameTest, RemovePiece_RemovePieceI1LineAndPiece)
{
	auto board = game.GetBoard();
	game.PlacePiece({0, 1});//r
	game.PlacePiece({0, 3});//n
	game.PlacePiece({1, 1});//r
	game.PlacePiece({1, 3});//n
	game.PlacePiece({2, 1});//r
	game.PlacePiece({2, 2});//n
	game.PlacePiece({0, 6});//r

	EXPECT_EQ(game.RemovePiece(Pos{0,6}),EOperationResult::NoError);//n
}

TEST_F(GameTest, RemovePiece_RemovePieceI2Line)
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);
	EXPECT_CALL(listener, OnRemovePiece(Pos{ 1,0 })).Times(1);

	game.LoadGame("Game2Lines.txt");

	EXPECT_EQ(game.RemovePiece({ 1,0 }), EOperationResult::NoError);//n
}

TEST_F(GameTest, ResetGame) {

	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnPlacePiece(Pos{ 1, 1 },0)).Times(1);

	game.PlacePiece({ 1, 1 });
	game.ResetGame();

	ASSERT_TRUE(game.GetGameState() == EState::Place);
	ASSERT_TRUE(game.GetCurrentPlayer() == EPlayer::Player1);
	ASSERT_TRUE(game.GetBoard()[1][1] == nullptr);
}

TEST_F(GameTest, MovePiece_CreatesLine)
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);
	EXPECT_CALL(listener, OnMovePiece(Pos{ 1, 5 }, Pos{ 2, 5 },0)).Times(1);
	EXPECT_CALL(listener, OnMovePiece(Pos{ 0, 3 }, Pos{ 1, 3 },1)).Times(1);

	game.LoadGame("GameLineMove.txt");
	game.MovePiece({ 1, 5 }, { 2, 5 });
	game.MovePiece({ 0, 3 }, { 1, 3 });
    EXPECT_TRUE(game.GetGameState() == EState::Capture);
}

TEST_F(GameTest, MovePiece_NoLine)
{
	EXPECT_CALL(listener, OnMovePiece(Pos{0, 1},Pos {2, 1},(int)game.GetCurrentPlayer())).Times(1);
	auto board = game.GetBoard();
	game.PlacePiece({0, 1});//r
	game.PlacePiece({0, 3});//n
	game.PlacePiece({1, 1});//r
	game.PlacePiece({1, 3});//n

	game.MovePiece(Pos{0, 1}, Pos{2, 1});
	EXPECT_TRUE(game.GetGameState() == EState::Place);

}

TEST_F(GameTest, AddListener) {
	MockGameListener listener1;
	game.AddListener(&listener1);

	EXPECT_CALL(listener, OnPlacePiece(Pos{ 1, 1 }, 0)).Times(1);
	EXPECT_CALL(listener, OnPlacePiece(Pos{ 1, 2 }, 1)).Times(1);
	EXPECT_CALL(listener1, OnPlacePiece(Pos{ 1, 1 }, 0)).Times(1);
	EXPECT_CALL(listener1, OnPlacePiece(Pos{ 1, 2 }, 1)).Times(1);

	EOperationResult result = game.PlacePiece(Pos{ 1, 1 });
	EOperationResult result2 = game.PlacePiece(Pos{ 1, 2 });

	EXPECT_EQ(result, EOperationResult::NoError);
	EXPECT_EQ(result2, EOperationResult::NoError);

}

TEST_F(GameTest, RemoveListener) {
	EXPECT_CALL(listener, OnPlacePiece(Pos{ 1,1 }, 0)).Times(0);

	game.RemoveListener(&(this->listener));
	EOperationResult result = game.PlacePiece({ 1, 1 });


	EXPECT_EQ(result, EOperationResult::NoError);
}

TEST_F(GameTest, WhereToMove_4Pieces)
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);

	game.LoadGame("WhereToMove4.txt");
    std::vector<std::pair<int, int>> expectedMoves = { {0, 1}, {0, 7} };
    
	EXPECT_EQ(expectedMoves, game.WhereToMove({ 0, 0 }));
}

TEST_F(GameTest, WhereToCapture)
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);
	EXPECT_CALL(listener, OnPlacePiece(Pos{1,7},0)).Times(1);
	EXPECT_CALL(listener, OnPlacePiece(Pos{ 1,4 },1)).Times(1);


	game.LoadGame("GameCaptureAndMove.txt");
	std::vector<std::pair<int, int>> expectedMoves = { { 1,0}, {1, 1},{1, 4},{1, 6} };

	game.PlacePiece({ 1, 7 });//1
	game.PlacePiece({ 1, 4 });//2
	EXPECT_EQ(expectedMoves, game.WhereToCapture());
}

TEST_F(GameTest, WhereToMove_3Pieces)
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);

	game.LoadGame("GameCaptureAndMove.txt");
	EXPECT_EQ(game.WhereToMove({ 0, 0 }).size(), 18);
}

TEST_F(GameTest, WhereToMove_Blocaj)//no
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);

	game.LoadGame("GameBlocaj.txt");

	EXPECT_EQ(0, game.WhereToMove({ 0, 0 }).size());
}

TEST_F(GameTest, SetStartegy_MediumStrategy)//no
{
    game.SetStrategy(EGameType::Medium);
}

TEST_F(GameTest, SetStartegy_CustomStrategy)
{
	EXPECT_CALL(listener, OnPlacePiece(Pos{ 1,1 },0)).Times(1);
	IStrategyPtr strategy = std::make_shared<CustomStrategy>(game.GetBoard());
	game.SetStrategy(strategy);
	game.PlacePiece({ 1, 1 });
}
TEST_F(GameTest, SetStartegy_EasyStrategy)
{
	game.SetStrategy(EGameType::Easy);
}

TEST_F(GameTest, Undo_Place)//no
{
	EXPECT_CALL(listener, OnPlacePiece(Pos{ 0,0 }, 0)).Times(1);
	Positions pos{ {0,0} };
	EXPECT_CALL(listener, OnUndo(pos)).Times(1);
	

	game.PlacePiece({ 0,0 });//1
	game.Undo();
	EXPECT_FALSE(game.GetBoard()[0][0]);
	EXPECT_TRUE(game.GetCurrentPlayer() == EPlayer::Player1);
}

TEST_F(GameTest, Undo_NewGame)//no
{
	EXPECT_FALSE(game.Undo());
}

TEST_F(GameTest, Undo_WinGame)//no
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);
	EXPECT_CALL(listener, OnPlacePiece(Pos{0,0},0)).Times(1);

	game.LoadGame("GameStateWin.txt");
	game.PlacePiece({ 0,0 });
	EXPECT_FALSE(game.Undo());
}

TEST_F(GameTest, Load_NoFile)//no
{
	EXPECT_CALL(listener, OnReset()).Times(1);

	EXPECT_FALSE(game.LoadGame("kkkkk"));
}

TEST_F(GameTest, Blocaj)//no
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);
	EXPECT_CALL(listener, OnMovePiece(Pos{ 2, 5 },	Pos{ 1, 5 }, 1)).Times(1);

	game.LoadGame("GameBlocaj.txt");
	game.MovePiece({ 2, 5 }, { 1, 5 });
	EXPECT_EQ(game.GetGameState(), EState::Player2Won);
}

TEST_F(GameTest, MovePiece_NoPiece)//no
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);

	game.LoadGame("GameBlocaj.txt");
	EXPECT_EQ(game.MovePiece({ 3, 7 }, { 1, 5 }), EOperationResult::IsNotAValidPiece);
}

TEST_F(GameTest, MovePiece_NotYourPiece)//no
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);

	game.LoadGame("GameBlocaj.txt");
	EXPECT_NE(game.MovePiece({ 0, 0 }, { 1, 5 }), EOperationResult::NoError);
}

TEST_F(GameTestComputer, PlacePiece_RobotChoiceMedium)//no
{
	EXPECT_CALL(listener, OnPlacePiece).Times(2);

	game->SetStrategy(EGameType::Medium);
	game->PlacePiece({ 0,0 });
	

	EXPECT_EQ(game->GetGameState(), EState::Place);
	EXPECT_TRUE(game->GetBoard()[0][0] != nullptr);
}

TEST_F(GameTestComputer, PlacePiece_RobotChoiceEasy)//no
{
	EXPECT_CALL(listener, OnPlacePiece).Times(2);

	game->SetStrategy(EGameType::Easy);
	game->PlacePiece({ 0,0 });
	EXPECT_EQ(game->GetGameState(), EState::Place);
	EXPECT_TRUE(game->GetBoard()[0][0] != nullptr);
}

//TEST_F(GameTestComputer, CapturePiece_RobotChoiceEasy)//no
//{
//	EXPECT_CALL(listener, OnReset()).Times(1);
//	EXPECT_CALL(listener, OnLoad()).Times(1);
//	EXPECT_CALL(listener, OnRemovePiece).Times(1);
//
//	game->LoadGame("GameStateCapture.txt");
//	game->SetStrategy(EGameType::Easy);
//	game->MakeComputerAction();
//	EXPECT_EQ(game->GetGameState(), EState::Place);
//}

//TEST_F(GameTestComputer, CapturePiece_RobotChoiceMedium)//no
//{
//	EXPECT_CALL(listener, OnReset()).Times(1);
//	EXPECT_CALL(listener, OnLoad()).Times(1);
//	EXPECT_CALL(listener, OnRemovePiece).Times(1);
//
//	game->LoadGame("GameStateCapture.txt");
//	game->SetStrategy(EGameType::Medium);
//	game->MakeComputerAction();
//	EXPECT_EQ(game->GetGameState(), EState::Place);
//}

TEST_F(GameTestComputer, MovePiece_RobotChoiceMedium)//no
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);
	EXPECT_CALL(listener, OnMovePiece).Times(2);

	game->LoadGame("GameStateOnlyMove.txt");
	game->SetStrategy(EGameType::Medium);
	game->MovePiece({ 0,0 }, { 0, 7 });
	EXPECT_TRUE(game->GetGameState() == EState::Move);
}

TEST_F(GameTestComputer, MovePiece_RobotChoiceEasy)//no
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);
	EXPECT_CALL(listener, OnMovePiece).Times(2);

	game->LoadGame("GameStateOnlyMove.txt");
	game->SetStrategy(EGameType::Easy);
	game->MovePiece({ 0,0 }, { 0, 7 });
	EXPECT_TRUE(game->GetGameState() == EState::Move);
}

//TEST_F(GameTestComputer, Win_RobotChoiceMedium)//no
//{
//	EXPECT_CALL(listener, OnReset()).Times(1);
//	EXPECT_CALL(listener, OnLoad()).Times(1);
//
//	game->LoadGame("GameStateWin.txt");
//	game->SetStrategy(EGameType::Medium);
//	game->MakeComputerAction();
//	EXPECT_EQ(game->MakeComputerAction(), EOperationResult::NoError);
//}

TEST_F(GameTestComputer, MovePieceMakeLine_RobotChoiceMedium)
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);
	EXPECT_CALL(listener, OnMovePiece(Pos{ 0,0 }, Pos{ 0,7 }, 0));
	EXPECT_CALL(listener, OnMovePiece(Pos{ 1,3 }, Pos{ 2,3 }, 1));
	EXPECT_CALL(listener, OnRemovePiece).Times(1);

	game->LoadGame("GameRobotMediumMakeLine.txt");
	game->SetStrategy(EGameType::Medium);
	game->MovePiece({ 0,0 }, { 0,7 });
}

TEST_F(GameTestComputer, PlacePieceMakeLine_RobotChoiceMedium)
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);
	EXPECT_CALL(listener, OnPlacePiece(Pos{ 2,0 }, 0));
	EXPECT_CALL(listener, OnPlacePiece(Pos{ 1,2 }, 1));
	EXPECT_CALL(listener, OnRemovePiece).Times(1);


	game->LoadGame("GameRobotPlaceLine.txt");
	game->SetStrategy(EGameType::Medium);
	game->PlacePiece({ 2,0 });
}

TEST_F(GameTestComputer, PlacePieceStopLine_RobotChoiceMedium)
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);
	EXPECT_CALL(listener, OnPlacePiece(Pos{ 0,1 }, 0));
	EXPECT_CALL(listener, OnPlacePiece(Pos{ 0,2 }, 1));

	game->LoadGame("GameStopLine.txt");
	game->SetStrategy(EGameType::Medium);
	game->PlacePiece({ 0,1 });
}

TEST_F(GameTestComputer, MovePieceStopLine_RobotChoiceMedium)
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);
	EXPECT_CALL(listener, OnMovePiece).Times(2);

	game->LoadGame("GameMoveStopLine.txt");
	game->SetStrategy(EGameType::Medium);
	game->MovePiece({ 1,5 }, { 0,5 });
}
//
//TEST_F(GameTestComputer, MovePieceRandom_RobotChoiceMedium)
//{
//	EXPECT_CALL(listener, OnReset()).Times(1);
//	EXPECT_CALL(listener, OnLoad()).Times(1);
//	EXPECT_CALL(listener, OnMovePiece);
//
//	game->LoadGame("GameRobotMediumRandom.txt");
//	game->SetStrategy(EGameType::Medium);
//	game->MakeComputerAction();
//}//noooooooo

//TEST_F(GameTestComputer, Win_RobotChoiceEasy)//no
//{
//	EXPECT_CALL(listener, OnReset()).Times(1);
//	EXPECT_CALL(listener, OnLoad()).Times(1);
//
//	game->LoadGame("GameStateWin.txt");
//	game->SetStrategy(EGameType::Easy);
//	game->MakeComputerAction();
//	EXPECT_EQ(game->MakeComputerAction(), EOperationResult::NoError);
//}//noooo

TEST_F(GameTestComputer, PlacePiece_RobotChoiceMediumMakeLine)//no
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);
	EXPECT_CALL(listener, OnPlacePiece).Times(2);
	EXPECT_CALL(listener, OnRemovePiece).Times(1);

	game->SetStrategy(EGameType::Medium);
	game->LoadGame("GameMakeLine.txt");
	
	EXPECT_EQ(game->PlacePiece({ 0, 2 }), EOperationResult::NoError);
} 
TEST_F(GameTestComputer, PlacePiece_RobotChoiceMediumBlockLine)//no warn
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);
	EXPECT_CALL(listener, OnPlacePiece).Times(2);

	game->SetStrategy(EGameType::Medium);
	game->LoadGame("GameBlockLine.txt");

	EXPECT_EQ(game->PlacePiece({ 0, 6 }), EOperationResult::NoError);
}
TEST_F(GameTest, MovePiece_block)//no warn
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);
	EXPECT_CALL(listener, OnMovePiece).Times(1);
	game.LoadGame("GameBlocaj.txt");

	EXPECT_EQ(game.MovePiece({ 2, 5 }, { 1, 5 }), EOperationResult::NoError);
}

TEST_F(GameTest, Undo_MovePiece)//no warn
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);
	EXPECT_CALL(listener, OnMovePiece).Times(1);
	EXPECT_CALL(listener, OnUndo).Times(1);

	game.LoadGame("GameUndoMove.txt");
	
	EXPECT_EQ(game.MovePiece({ 2, 5 }, { 1, 5 }), EOperationResult::NoError);
	EXPECT_EQ(game.Undo(),true);
}

TEST_F(GameTest, Undo_CapturePiece)//no warn
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);
	EXPECT_CALL(listener, OnRemovePiece(Pos{ 0,4 })).Times(1);
	EXPECT_CALL(listener, OnUndo).Times(1);

	game.LoadGame("GameStateMove.txt");

	EXPECT_EQ(game.RemovePiece({ 0, 4 }), EOperationResult::NoError);
	EXPECT_EQ(game.Undo(), true);
}

TEST_F(GameTest, GetTime_Test)
{
	EXPECT_EQ(game.GetTime(EPlayer::Player1), Miliseconds(60 * 
		0));
	EXPECT_EQ(game.GetTime(EPlayer::Player2), Miliseconds(60 * 1000));
}

TEST_F(GameTest, StartStopTimer_Player1Won)
{
	game.Start();
	game.Stop();
	EXPECT_EQ(game.GetGameState(), EState::Player2Won);
}

TEST_F(GameTest, StartStopTimer_Player2Won)
{
	EXPECT_CALL(listener, OnPlacePiece(Pos{ 0,0 }, 0));
	game.Start();
	game.PlacePiece({ 0,0 });
	game.Stop();
	EXPECT_EQ(game.GetGameState(), EState::Player1Won);
}

TEST_F(GameTestComputer, CapturePiece_RobotChoiceEasy)
{
	EXPECT_CALL(listener, OnReset()).Times(1);
	EXPECT_CALL(listener, OnLoad()).Times(1);

	game->SetStrategy(EGameType::Easy);
	game->LoadGame("GameRobotEasyCapture.txt");
	game->PlacePiece({ 2,7 });
}

//TEST_F(GameTestComputer, CapturePiece_RobotChoiceMedium)//no warn
//{
//	EXPECT_CALL(listener, OnReset()).Times(1);
//	EXPECT_CALL(listener, OnLoad()).Times(1);
//	EXPECT_CALL(listener, OnMovePiece(Pos{ 2, 6 }, Pos{ 2, 7 }, 0)).Times(1);
//	EXPECT_CALL(listener, OnRemovePiece).Times(1);
//	
//	game->LoadGame("GameCapture.txt");
//	int curentPieces = 0;
//	for (auto elem : board)
//	{
//		for (auto e : elem)
//		{
//			if (e && e->GetColor() == EColor::Red)
//				curentPieces++;
//		}
//	}
//	game->SetStrategy(EGameType::Medium);
//	game->MovePiece({ 2, 6 }, { 2, 7 });
//
//	for (auto elem : board)
//	{
//		for (auto e : elem)
//		{
//			if (e && e->GetColor() == EColor::Red)
//				curentPieces--;
//		}
//	}
//
//	EXPECT_EQ(curentPieces, 1);
//}

