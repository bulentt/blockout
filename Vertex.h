#pragma once

struct Vertex
{
	Vertex(const Vector3& pos)
		: Position(pos)
	{
	}

	Vertex(float x, float y, float z)
		: Position(Vector3(x, y ,z))
	{
	}
	
	Vector3	Position;
};
