#include <gtest/gtest.h>
#include <memory>

#include "Board.h"
#include "Piece.h"

using IPiecePtr = std::shared_ptr<IPiece>;
class BoardTest : public ::testing::Test {
protected:
	Board board;
	IPiecePtr piece1 = std::make_shared<Piece>(EColor::Red, 1, 2);

	BoardTest() : board(3, 8) {

		board.PlacePiece(piece1);
	}
};

TEST_F(BoardTest, StillInitialize_NotDone)
{
	EXPECT_TRUE(board.StillInitialize());
}

TEST_F(BoardTest, PossibleCapture_OpponentHasThreeOrFewerPieces) 
{
	// Set up scenario where the opponent has 3 pieces
	IPiecePtr piece2 = std::make_shared<Piece>(EColor::Black, 1, 1);
	IPiecePtr piece3 = std::make_shared<Piece>(EColor::Black, 0, 0);
	IPiecePtr piece9 = std::make_shared<Piece>(EColor::Black, 2, 2);

    board.PlacePiece(piece2);
    board.PlacePiece(piece3);
    board.PlacePiece(piece9);

	Positions moves = board.PossibleCapture(0);

	// All opponent positions should be available for capture
	EXPECT_EQ(moves.size(), 3);
	EXPECT_NE(std::find(moves.begin(), moves.end(), std::make_pair(0, 0)), moves.end());
	EXPECT_NE(std::find(moves.begin(), moves.end(), std::make_pair(1, 1)), moves.end());
	EXPECT_NE(std::find(moves.begin(), moves.end(), std::make_pair(2, 2)), moves.end());
}

TEST_F(BoardTest, PossibleCapture_AllOpponentPiecesInLines) 
{
	// Mock CheckAllLines to return true
	IPiecePtr piece2 = std::make_shared<Piece>(EColor::Black, 0, 1);
	IPiecePtr piece3 = std::make_shared<Piece>(EColor::Black, 0, 0);
	IPiecePtr piece9 = std::make_shared<Piece>(EColor::Black, 0, 2);

	board.PlacePiece(piece2);
	board.PlacePiece(piece3);
	board.PlacePiece(piece9);

	Positions moves = board.PossibleCapture(0);

	// All opponent positions should be available for capture
	EXPECT_EQ(moves.size(), 3);
	EXPECT_NE(std::find(moves.begin(), moves.end(), std::make_pair(0, 0)), moves.end());
	EXPECT_NE(std::find(moves.begin(), moves.end(), std::make_pair(0, 1)), moves.end());
	EXPECT_NE(std::find(moves.begin(), moves.end(), std::make_pair(0, 2)), moves.end());
}

TEST_F(BoardTest, PossibleCapture_NormalCaptureLogic) 
{
	// Mock CheckLine to return false for one piece and true for others
	IPiecePtr piece2 = std::make_shared<Piece>(EColor::Black, 0, 1);
	IPiecePtr piece3 = std::make_shared<Piece>(EColor::Black, 0, 0);
	IPiecePtr piece9 = std::make_shared<Piece>(EColor::Black, 0, 2);
	IPiecePtr piece4 = std::make_shared<Piece>(EColor::Black, 1, 5);

	board.PlacePiece(piece2);
	board.PlacePiece(piece3);
	board.PlacePiece(piece9);
	board.PlacePiece(piece4);

	Positions moves = board.PossibleCapture(0);

	// Only the piece at (0, 0) should be available for capture
	EXPECT_EQ(moves.size(), 1);
	EXPECT_NE(std::find(moves.begin(), moves.end(), std::make_pair(1, 5)), moves.end());
}

TEST_F(BoardTest, PossibleMove_FewerThanFourPieces) 
{
	
	IPiecePtr piece4 = std::make_shared<Piece>(EColor::Red, 0, 0);
	IPiecePtr piece5 = std::make_shared<Piece>(EColor::Red, 0, 7);
	
	board.PlacePiece(piece4);
	board.PlacePiece(piece5);
	
	// All positions should be available
	Positions moves = board.PossibleMove(0, { 1, 2 });
	EXPECT_EQ(moves.size(), 21);
	
}

TEST_F(BoardTest, PossibleMove_FourOrMorePieces) 
{
	// Set up a scenario where the current player has 4 or more pieces
	IPiecePtr piece4 = std::make_shared<Piece>(EColor::Red, 0, 0);
	IPiecePtr piece5 = std::make_shared<Piece>(EColor::Red, 0, 7);
	IPiecePtr piece6 = std::make_shared<Piece>(EColor::Red, 0, 6);
	board.PlacePiece(piece4);
	board.PlacePiece(piece5);
	board.PlacePiece(piece6);

	Positions moves = board.PossibleMove(0, { 1, 2 });
	EXPECT_EQ(moves.size(), 2);

	EXPECT_NE(std::find(moves.begin(), moves.end(), std::make_pair(1, 1)), moves.end());
	EXPECT_NE(std::find(moves.begin(), moves.end(), std::make_pair(1, 3)), moves.end());
}

TEST_F(BoardTest, GetPiece_PieceExists)
{
	EXPECT_TRUE(board.GetPiece({ 1, 2 }));
}

TEST_F(BoardTest, GetPiece_PieceNoExists)
{
	EXPECT_FALSE(board.GetPiece({ -1, 3 }));
}

TEST_F(BoardTest, Blocaj_Player1Blocked)
{
	IPiecePtr piece2 = std::make_shared<Piece>(EColor::Black, 1, 1);
	IPiecePtr piece3 = std::make_shared<Piece>(EColor::Black, 1, 3);
	IPiecePtr piece9 = std::make_shared<Piece>(EColor::Black, 0, 1);
	IPiecePtr piece7 = std::make_shared<Piece>(EColor::Black, 0, 5);
	IPiecePtr piece8 = std::make_shared<Piece>(EColor::Black, 1, 7);
	IPiecePtr piece4 = std::make_shared<Piece>(EColor::Red, 0, 0);
	IPiecePtr piece5 = std::make_shared<Piece>(EColor::Red, 0, 7);
	IPiecePtr piece6 = std::make_shared<Piece>(EColor::Red, 0, 6);
	board.PlacePiece(piece2);
	board.PlacePiece(piece3);
	board.PlacePiece(piece4);
	board.PlacePiece(piece5);
	board.PlacePiece(piece6);
	board.PlacePiece(piece7);
	board.PlacePiece(piece8);
	board.PlacePiece(piece9);
	EXPECT_TRUE(board.Blocaj(0));
}

TEST_F(BoardTest, Blocaj_Player1NotBlocked)
{
	IPiecePtr piece2 = std::make_shared<Piece>(EColor::Black, 1, 1);
	board.PlacePiece(piece2);
	EXPECT_FALSE(board.Blocaj(1));
}

TEST_F(BoardTest, RemovePiece_RemoveRivalPieceTest)
{
	board.RemovePiece({ 1, 2 }, 1);
	EXPECT_EQ(nullptr, board.GetBoard()[1][2]);
}

TEST_F(BoardTest, RemovePiece_RemovePieceOutOfBoard)
{

	EXPECT_EQ(board.RemovePiece({ 2, 2 }, 1), EOperationResult::IsNotAValidPiece);
}

TEST_F(BoardTest, RemovePiece_RemoveNoPieceTest)
{
	EXPECT_EQ(board.RemovePiece({ 1, 2 }, 0), EOperationResult::IsNotAValidPiece);
}

TEST_F(BoardTest, RemovePiece_RemoveYourPieceTest)
{
	EXPECT_NE(board.RemovePiece({ 1, 2 }, 0), EOperationResult::NoError);
}

TEST_F(BoardTest, RemovePiece_RemoveOnlyLines)
{
	IPiecePtr piece2 = std::make_shared<Piece>(EColor::Red, 0, 0);
	IPiecePtr piece3 = std::make_shared<Piece>(EColor::Red, 0, 7);
	IPiecePtr piece4 = std::make_shared<Piece>(EColor::Red, 0, 6);
	IPiecePtr piece5 = std::make_shared<Piece>(EColor::Red, 1, 1);
	IPiecePtr piece6 = std::make_shared<Piece>(EColor::Red, 1, 2);
	IPiecePtr piece7 = std::make_shared<Piece>(EColor::Red, 1, 0);
	board.PlacePiece(piece2);
	board.PlacePiece(piece3);
	board.PlacePiece(piece4);
	board.PlacePiece(piece5);
	board.PlacePiece(piece6);
	board.PlacePiece(piece7);
	EXPECT_EQ(board.RemovePiece({ 0, 6 }, (int)EColor::Black), EOperationResult::NoError);
}

TEST_F(BoardTest, RemovePiece_RemovePieceNoLine)
{
	IPiecePtr piece2 = std::make_shared<Piece>(EColor::Red, 0, 0);
	IPiecePtr piece3 = std::make_shared<Piece>(EColor::Red, 0, 7);
	IPiecePtr piece4 = std::make_shared<Piece>(EColor::Red, 0, 6);
	IPiecePtr piece5 = std::make_shared<Piece>(EColor::Red, 1, 1);

	board.PlacePiece(piece2);
	board.PlacePiece(piece3);
	board.PlacePiece(piece4);
	board.PlacePiece(piece5);

	EXPECT_EQ(board.RemovePiece({ 1, 1 }, (int)EColor::Black), EOperationResult::NoError);
}

TEST_F(BoardTest, MovePiece_NotYourPiece)
{
	EXPECT_EQ(board.MovePiece({ 1, 1 }, piece1, (int)EColor::Black), EOperationResult::IsNotAValidPiece);
}

TEST_F(BoardTest, MovePiece_BadMove)
{
	IPiecePtr piece2 = std::make_shared<Piece>(EColor::Red, 0, 0);
	IPiecePtr piece3 = std::make_shared<Piece>(EColor::Red, 0, 7);
	IPiecePtr piece4 = std::make_shared<Piece>(EColor::Red, 0, 6);
	board.PlacePiece(piece2);
	board.PlacePiece(piece3);
	board.PlacePiece(piece4);
	EXPECT_EQ(board.MovePiece({ 1, 6 }, piece2, (int)EColor::Red), EOperationResult::IsNotAValidPlace);
}

TEST_F(BoardTest, GetNrPieces_GetNrPiecesOneRed)
{
	EXPECT_EQ(board.GetNrPieces((int)EColor::Red), 1);
	EXPECT_EQ(board.GetNrPieces((int)EColor::Black), 0);
}

TEST_F(BoardTest, SetPiece_IsTrue)
{
	IPiecePtr piece4 = std::make_shared<Piece>(EColor::Red, 2, 2);
	board.SetPiece(0, 0, piece4);
	EXPECT_EQ(board.GetPiece({ 0, 0 }), piece4);
	EXPECT_FALSE(board.GetPiece({ 2, 2 }) == piece4);
}

TEST_F(BoardTest, SetNrLinesPisces)
{
	board.SetNrLines(0, 1);
	EXPECT_EQ(board.GetNrLines((int)EColor::Red), 1);
	board.SetNrPieces(1, 12);
	EXPECT_EQ(board.GetNrPieces((int)EColor::Black), 12);


}

TEST_F(BoardTest, GetNrLines_NoLine)
{
	EXPECT_EQ(board.GetNrLines((int)EColor::Red), 0);

}

TEST_F(BoardTest, GetNrLines_OneLine)
{
	IPiecePtr piece4 = std::make_shared<Piece>(EColor::Red, 0, 0);
	IPiecePtr piece5 = std::make_shared<Piece>(EColor::Red, 0, 7);
	IPiecePtr piece6 = std::make_shared<Piece>(EColor::Red, 0, 6);
	board.PlacePiece(piece4);
	board.PlacePiece(piece5);
	board.PlacePiece(piece6);

	EXPECT_EQ(board.GetNrLines((int)EColor::Red), 1);

}

TEST_F(BoardTest, GetNrPieces_GetNrPiecesOneBlack)
{
	EXPECT_EQ(board.GetNrPieces((int)EColor::Red), 1);
	EXPECT_EQ(board.GetNrPieces((int)EColor::Black), 0);
}

TEST_F(BoardTest, GetBoard) {
	auto boardMatrix = board.GetBoard();
	EXPECT_EQ(boardMatrix.size(), 3);
	for (const auto& row : boardMatrix) {
		EXPECT_EQ(row.size(), 8);
	}
}

TEST_F(BoardTest, BlackOverRed_isFalse)
{
	IPiecePtr r = std::make_shared<Piece>(EColor::Black, 1, 2);
	EXPECT_NE(board.PlacePiece(r), EOperationResult::NoError);
}

TEST_F(BoardTest, RedOverRed_isFalse)
{
	IPiecePtr r = std::make_shared<Piece>(EColor::Red, 1, 2);
	EXPECT_NE(board.PlacePiece(r), EOperationResult::NoError);
}

TEST_F(BoardTest, RightPlace_isTrue)
{
	EXPECT_EQ(board.GetBoard()[1][2]->GetColor(), piece1->GetColor());
}

TEST_F(BoardTest, CheckLine_LineOneDiff)
{
	IPiecePtr r = std::make_shared<Piece>(EColor::Black, 1, 3);
	IPiecePtr q = std::make_shared<Piece>(EColor::Red, 1, 4);
	board.PlacePiece(r);
	board.PlacePiece(q);

	EXPECT_FALSE(board.CheckLine(piece1->GetPozition(), (int)piece1->GetColor()));
}

TEST_F(BoardTest, CheckLine_LineFullSameVertical)
{
	IPiecePtr r = std::make_shared<Piece>(EColor::Red, 1, 3);
	IPiecePtr q = std::make_shared<Piece>(EColor::Red, 1, 4);
	board.PlacePiece(r);
	board.PlacePiece(q);

	EXPECT_TRUE(board.CheckLine(piece1->GetPozition(), (int)piece1->GetColor()));
}

TEST_F(BoardTest, CheckLine_LineFullSameOdd)
{
	IPiecePtr p = std::make_shared<Piece>(EColor::Red, 1, 1);
	IPiecePtr r = std::make_shared<Piece>(EColor::Red, 2, 1);
	IPiecePtr q = std::make_shared<Piece>(EColor::Red, 0, 1);

	board.PlacePiece(p);
	board.PlacePiece(r);
	board.PlacePiece(q);

	EXPECT_TRUE(board.CheckLine(p->GetPozition(), (int)p->GetColor()));
}

TEST_F(BoardTest, CheckLine_LineFullSameHorizontal)
{
	IPiecePtr p = std::make_shared<Piece>(EColor::Red, 1, 1);
	IPiecePtr r = std::make_shared<Piece>(EColor::Red, 1, 0);
	board.PlacePiece(p);
	board.PlacePiece(r);
	EXPECT_TRUE(board.CheckLine(p->GetPozition(), (int)p->GetColor()));
}

TEST_F(BoardTest, CheckLine_LineFullSameEven) {
	IPiecePtr p = std::make_shared<Piece>(EColor::Red, 0, 0);
	IPiecePtr r = std::make_shared<Piece>(EColor::Red, 1, 0);
	IPiecePtr q = std::make_shared<Piece>(EColor::Red, 2, 0);

	board.PlacePiece(p);
	board.PlacePiece(r);
	board.PlacePiece(q);

	EXPECT_FALSE(board.CheckLine(p->GetPozition(), (int)p->GetColor()));
}

TEST_F(BoardTest, CheckLine_Only2Pieces) {
	IPiecePtr p = std::make_shared<Piece>(EColor::Red, 0, 0);
	IPiecePtr r = std::make_shared<Piece>(EColor::Red, 1, 0);

	board.PlacePiece(p);
	board.PlacePiece(r);

	EXPECT_FALSE(board.CheckLine(p->GetPozition(), (int)p->GetColor()));
}

TEST_F(BoardTest, CheckLine_Only1Piece) {
	IPiecePtr p = std::make_shared<Piece>(EColor::Red, 0, 0);
	board.PlacePiece(p);
	EXPECT_FALSE(board.CheckLine(p->GetPozition(), (int)p->GetColor()));
}

TEST_F(BoardTest, PlacePiece_OutOfBoard) {
	IPiecePtr p = std::make_shared<Piece>(EColor::Red, 1, 8);
	IPiecePtr q = std::make_shared<Piece>(EColor::Red, -1, 7);
	EXPECT_NE(board.PlacePiece(p), EOperationResult::NoError);
	EXPECT_NE(board.PlacePiece(q), EOperationResult::NoError);

}

TEST_F(BoardTest, MovePiece_Valid) {

	EXPECT_EQ(board.MovePiece({ 1, 3 }, piece1, 0), EOperationResult::NoError);
	EXPECT_FALSE(board.GetBoard()[1][2]);
	ASSERT_TRUE(board.GetBoard()[1][3] != nullptr);
	EXPECT_EQ(board.GetBoard()[1][3]->GetColor(), EColor::Red);
}

TEST_F(BoardTest, MovePiece_OccupiedSpot)
{
	IPiecePtr piece2 = std::make_shared<Piece>(EColor::Black, 1, 3);
	board.PlacePiece(piece2);

	EXPECT_EQ(board.MovePiece({ 1, 3 }, piece1, 0), EOperationResult::IsNotAValidPlace);
	EXPECT_TRUE(board.GetBoard()[1][2]);
	ASSERT_TRUE(board.GetBoard()[1][3]);
	EXPECT_EQ(board.GetBoard()[1][3]->GetColor(), EColor::Black);
	EXPECT_EQ(board.GetBoard()[1][2]->GetColor(), EColor::Red);
}

TEST_F(BoardTest, MovePiece_SameSpot)
{
	EXPECT_EQ(board.MovePiece({ 1, 2 }, piece1, 0), EOperationResult::IsNotAValidPlace);
	EXPECT_TRUE(board.GetBoard()[1][2]);
}

TEST_F(BoardTest, MovePiece_OccupiedSpotSameColor)
{
	IPiecePtr piece2 = std::make_shared<Piece>(EColor::Red, 1, 3);
	board.PlacePiece(piece2);

	EXPECT_EQ(board.MovePiece({ 1, 3 }, piece1, 0), EOperationResult::IsNotAValidPlace);
	EXPECT_TRUE(board.GetBoard()[1][2]);
	EXPECT_TRUE(board.GetBoard()[1][3]);
}


TEST_F(BoardTest, GetNrTotalPieces)
{
	board.PlacePiece(piece1);
	EXPECT_EQ(board.GetNrTotalPieces(), 17);
}

TEST_F(BoardTest, SetNrTotalPieces)
{
	board.SetNrTotalPieces(10);
	EXPECT_EQ(board.GetNrTotalPieces(), 10);
}

TEST_F(BoardTest, PlacePiece_NoError)
{
	IPiecePtr piece2 = std::make_shared<Piece>(EColor::Red, 1, 3);
	EXPECT_EQ(board.PlacePiece(piece2), EOperationResult::NoError);
}

TEST_F(BoardTest, UndoMove_NoLine) {
	int initialRow = piece1->GetPozition().first;
	int initialCol = piece1->GetPozition().second;

	board.MovePiece({ 2, 3 }, piece1, (int)EColor::Red);
	board.UndoMove({ initialRow, initialCol }, piece1, (int)EColor::Red);

	EXPECT_EQ(board.GetPiece({ initialRow, initialCol }), piece1);
	EXPECT_EQ(board.GetPiece({ 2, 3 }), nullptr);
}

TEST_F(BoardTest, UndoCapture_Simple)
{
	IPiecePtr p = std::make_shared<Piece>(EColor::Red, 1, 2);
	IPiecePtr r = std::make_shared<Piece>(EColor::Red, 2, 1);
	IPiecePtr q = std::make_shared<Piece>(EColor::Red, 0, 1);
	board.PlacePiece(p);
	board.PlacePiece(r);
	board.PlacePiece(q);
	board.RemovePiece({ 1, 2 }, 0);
	board.UndoCapture(p, 0);
	EXPECT_EQ(board.GetPiece({ 1, 2 }), p);
}

TEST_F(BoardTest, UndoCapture_Line)
{
	IPiecePtr p = std::make_shared<Piece>(EColor::Red, 1, 2);
	IPiecePtr r = std::make_shared<Piece>(EColor::Red, 1, 3);
	IPiecePtr q = std::make_shared<Piece>(EColor::Red, 1, 4);
	board.PlacePiece(p);
	board.PlacePiece(r);
	board.PlacePiece(q);
	board.RemovePiece({ 1, 2 }, 0);
	board.UndoCapture(p, 0);
	EXPECT_EQ(board.GetPiece({ 1, 2 }), p);
}

TEST_F(BoardTest, UndoPlace_Line)
{
	IPiecePtr p = std::make_shared<Piece>(EColor::Red, 1, 2);
	IPiecePtr r = std::make_shared<Piece>(EColor::Red, 1, 3);
	IPiecePtr q = std::make_shared<Piece>(EColor::Red, 1, 4);
	board.PlacePiece(p);
	board.PlacePiece(r);
	board.PlacePiece(q);
	board.UndoPlace({ 1, 2 }, 0);
	EXPECT_EQ(board.GetPiece({ 1, 2 }), nullptr);
}


TEST_F(BoardTest, UndoMove_CheckLinesDecremented) {
	IPiecePtr p = std::make_shared<Piece>(EColor::Red, 0, 2);
	IPiecePtr r = std::make_shared<Piece>(EColor::Red, 1, 1);
	IPiecePtr q = std::make_shared<Piece>(EColor::Red, 2, 1);
	board.PlacePiece(p);
	board.PlacePiece(r);
	board.PlacePiece(q);

	board.MovePiece({ 0, 1 }, p, (int)EColor::Red);
	EXPECT_EQ(board.GetNrLines((int)EColor::Red), 1);

	board.UndoMove({ 0, 2 }, p, (int)EColor::Red);
	EXPECT_EQ(board.GetNrLines((int)EColor::Red), 0);
}
