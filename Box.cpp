#include "pch.h"
#include "Box.h"
#include "Grid.h"

Box::Box(const Color& color /* = BLACK */) 
	: GameObject(color)
{
}

void Box::Initialize()
{
	// CREATE THE VERTEX BUFFER

	Vertex vertices[] =
	{
		Vertex(0.0f, 0.0f, 0.0f),
		Vertex(0.0f, 1.0f, 0.0f),
		Vertex(1.0f, 1.0f, 0.0f),
		Vertex(1.0f, 0.0f, 0.0f),
		Vertex(0.0f, 0.0f, 1.0f),
		Vertex(0.0f, 1.0f, 1.0f),
		Vertex(1.0f, 1.0f, 1.0f),
		Vertex(1.0f, 0.0f, 1.0f),
    };

	CreateVertexBuffer(m_pVertexBuffer, vertices, VERTICES_COUNT);

	// CREATE THE TRIANGLES INDEX BUFFER

	unsigned trianglesIndices[] =
	{
		// top face
		0, 1, 2,
		0, 2, 3,
		// left face
		5, 1, 0,
		5, 0, 4,
		// front face
		4, 0, 3,
		4, 3, 7,
		// right face
		7, 3, 2,
		7, 2, 6,
		// back face
		6, 2, 1,
		6, 1, 5,
		// without bottom face
	};

	CreateIndexBuffer(m_pTrianglesIndexBuffer, trianglesIndices, 3 * TRIANGLES_COUNT);

	// CREATE THE LINES INDEX BUFFERS

	unsigned linesIndices[] =
	{
		0, 1,
		1, 2,
		2, 3,
		0, 3,
		0, 4,
		1, 5,
		2, 6,
		3, 7,
	};
	
	CreateIndexBuffer(m_pLinesIndexBuffer, linesIndices, 2 * LINES_COUNT);
}

void Box::ReleaseBuffers()
{
	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pTrianglesIndexBuffer);
	SafeRelease(m_pLinesIndexBuffer);
}

void Box::Draw() const
{
	SetWorldTransformation();
	SetVertexBuffer(m_pVertexBuffer);
	// draw cube
	DrawTriangles(m_pTrianglesIndexBuffer, TRIANGLES_COUNT, m_Color);
	// draw border
	DrawLines(m_pLinesIndexBuffer, LINES_COUNT, BLACK);
}

ID3D10Buffer* Box::m_pVertexBuffer = nullptr;
ID3D10Buffer* Box::m_pTrianglesIndexBuffer = nullptr;
ID3D10Buffer* Box::m_pLinesIndexBuffer = nullptr;
