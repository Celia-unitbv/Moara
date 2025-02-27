#include <gtest/gtest.h>
#include "Piece.h"


TEST(PieceSetterColor, isTrue)
{
	Piece q(EColor::Red, 1, 4);

	q.SetColour(1);

	ASSERT_TRUE(q.GetColor()==EColor::Black);

}



TEST(PieceSetterPosition, isTrue)
{
	Piece q(EColor::Red, 1, 4);

    q.SetPozition({ 2, 3 });

	ASSERT_TRUE(q.GetPozition().first == 2 && q.GetPozition().second==3);

}

TEST(PieceGetterPosition, isTrue)
{
	Piece q(EColor::Red, 1, 4);

	ASSERT_TRUE(q.GetPozition().first == 1 && q.GetPozition().second == 4);

}

TEST(PieceGetterColor, isTrue)
{
	Piece q(EColor::Red, 1, 4);

	ASSERT_TRUE(q.GetColor() == EColor::Red);

}

TEST(CheckPlace_0, isTrue) {
    Piece p(EColor::Red, 0, 0);
    EXPECT_FALSE(p.CheckPlace({ 1,0 }));//diag
    EXPECT_TRUE(p.CheckPlace({ 0, 7 }));//down
    EXPECT_TRUE(p.CheckPlace({ 0, 1 }));//right
}
TEST(CheckPlace_SamePlace, isFalse) {
	Piece p(EColor::Red, 0, 0);
    EXPECT_FALSE(p.CheckPlace({ 0, 0 }));//diag
	
}

TEST(CheckPlace_1, isTrue) {
    Piece p(EColor::Red, 1, 1);
    EXPECT_TRUE(p.CheckPlace({ 0,1 }));//up
    EXPECT_TRUE(p.CheckPlace({ 2, 1 }));//down
    EXPECT_TRUE(p.CheckPlace({ 1, 0 }));//left
    EXPECT_TRUE(p.CheckPlace({ 1, 2 }));//right
}

TEST(CheckPlace_7, isTrue) {
    Piece p(EColor::Red, 0, 7);
    EXPECT_TRUE(p.CheckPlace({ 0, 0 }));//up
    EXPECT_TRUE(p.CheckPlace({ 0, 6 }));//down
}

TEST(CheckPlace_4, isTrue) {
    Piece p(EColor::Black, 1, 4);
    EXPECT_TRUE(p.CheckPlace({ 1, 3 }));//up
    EXPECT_TRUE(p.CheckPlace({ 1, 5 }));//left
}

TEST(CreatePieceNoParameters, isTrue)
{
    Piece p;
    EXPECT_TRUE(p.GetColor()== EColor::Red);
}