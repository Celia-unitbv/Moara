#pragma once

#include <vector>

class Node;
using NodeList = std::vector<Node*>;
using Pos = std::pair<int, int>;

class Node
{
public:
	Node(int initialX, int initialY);

	//setter
	void SetConnections(const NodeList& aux);
	void SetType(int type);
	void SetXY(Pos coords);
	void SetMoves(const NodeList& moves);

	//getters
	int GetX();
	int GetY();
	Pos GetInitialPoz()const;
	int GetType()const;
	NodeList GetConnections() const;
	NodeList GetMoves() const;

private:
	Pos m_initialPoz;
	int m_type;

	Pos m_screenPoz;
	NodeList m_connections;
	NodeList m_moves;
};

