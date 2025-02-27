#include "Node.h"

Node::Node(int initialX, int initialY)
	: m_initialPoz({ initialX,initialY })
	, m_screenPoz({ 0,0 })
	, m_type(-1)
{

}

void Node::SetConnections(const NodeList& aux)
{
	m_connections = aux;
}

int Node::GetX()
{
	return m_screenPoz.first;
}

int Node::GetY()
{
	return m_screenPoz.second;
}

Pos Node::GetInitialPoz() const
{
	return m_initialPoz;
}

int Node::GetType() const
{
	return m_type;
}

void Node::SetType(int type)
{
	m_type = type;
}

void Node::SetXY(Pos coords)
{
	m_screenPoz = { (coords.first * 50 + 25) ,(coords.second * 80 - 45) };
}

NodeList Node::GetConnections() const
{
	return m_connections;
}

NodeList Node::GetMoves() const
{
	return m_moves;
}

void Node::SetMoves(const NodeList& moves)
{
	m_moves = moves;
}
