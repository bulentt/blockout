#pragma once

#include "GameObject.h"

class Box : public GameObject
{
public:
	Box(const Color& color = BLACK);

	static void Initialize();
	static void ReleaseBuffers();

	virtual void Draw() const;

private:
	static ID3D10Buffer* m_pVertexBuffer;
	static ID3D10Buffer* m_pTrianglesIndexBuffer;
	static ID3D10Buffer* m_pLinesIndexBuffer;

	static const size_t VERTICES_COUNT = 8;
	static const size_t TRIANGLES_COUNT = 10;
	static const size_t LINES_COUNT = 8;
};
