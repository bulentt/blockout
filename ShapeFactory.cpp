#include "pch.h"
#include "ShapeFactory.h"
#include "Shape.h"

using namespace std;

void ShapeFactory::LoadShapeSetFromFile( const std::string& fileName )
{
	ifstream file(fileName.c_str());

	if (file.fail())
	{
		::MessageBox(0, ("Missing file " + fileName).c_str(), "Error", MB_ICONERROR);
		exit(1);
	}

	size_t shapesCount;
	file >> shapesCount;

	m_Shapes.resize(shapesCount);

	for (size_t i = 0; i < shapesCount; ++i)
	{
		LoadShape(file, m_Shapes[i]);
	}

	file.close();
}

Shape* ShapeFactory::CreateShape( unsigned shapeKind )
{
	return NewShape(m_Shapes[shapeKind]);
}

Shape* ShapeFactory::CreateRandomShape()
{
	return NewShape(m_Shapes[rand() % m_Shapes.size()]);
}

void ShapeFactory::ReleaseBuffers()
{
	for (ShapesDataContainer::iterator it = m_Shapes.begin(); it != m_Shapes.end(); ++it)
	{
		SafeRelease(it->pVertexBuffer);
		SafeRelease(it->pLinesIndexBuffer);
		SafeRelease(it->pTrianglesIndexBuffer);
	}
}

void ShapeFactory::LoadShape( istream& stream, ShapeData& shapeData )
{
	// LOAD VERTICES

	size_t verticesCount;
	stream >> verticesCount;

	vector<Vertex> vertices;

	for (size_t i = 0; i < verticesCount; ++i)
	{
		Vector3 position;
		stream >> position;
		vertices.push_back(Vertex(position));
	}

	// LOAD TRIANGLE INDICES

	stream >> shapeData.TrianglesCount;

	vector<unsigned> triangleIndices(shapeData.TrianglesCount * 3);

	for (size_t i = 0; i < shapeData.TrianglesCount * 3; ++i)
	{
		stream >> triangleIndices[i];
	}

	// LOAD LINE INDICES

	stream >> shapeData.LinesCount;

	vector<unsigned> lineIndices(shapeData.LinesCount * 2);

	for (size_t i = 0; i < shapeData.LinesCount * 2; ++i)
	{
		stream >> lineIndices[i];
	}

	// LOAD COMPOUND CUBES POSITIONS

	size_t cubesCount;
	stream >> cubesCount;

	shapeData.CubesRelativePositions.resize(cubesCount);

	for (size_t i = 0; i < cubesCount; ++i)
	{
		stream >> shapeData.CubesRelativePositions[i];
	}

	// CREATE BUFFERS

	Shape::CreateVertexBuffer(shapeData.pVertexBuffer, &vertices.front(), verticesCount);
	Shape::CreateIndexBuffer(shapeData.pTrianglesIndexBuffer, &triangleIndices.front(), shapeData.TrianglesCount * 3);
	Shape::CreateIndexBuffer(shapeData.pLinesIndexBuffer, &lineIndices.front(), shapeData.LinesCount * 2);
}

Shape* ShapeFactory::NewShape(const ShapeData& shapeData)
{
	return new Shape(
		shapeData.pVertexBuffer,
		shapeData.pTrianglesIndexBuffer,
		shapeData.pLinesIndexBuffer,
		shapeData.TrianglesCount,
		shapeData.LinesCount,
		shapeData.CubesRelativePositions);
}

std::vector<ShapeFactory::ShapeData> ShapeFactory::m_Shapes;
