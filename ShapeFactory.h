#pragma once

class Shape;

class ShapeFactory
{
public:
	static void LoadShapeSetFromFile(const std::string& fileName);
	static Shape* CreateShape(unsigned shapeKind);
	static Shape* CreateRandomShape();

	static void ReleaseBuffers();

private:

	struct ShapeData
	{
		ID3D10Buffer* pVertexBuffer;
		ID3D10Buffer* pTrianglesIndexBuffer;
		ID3D10Buffer* pLinesIndexBuffer;

		size_t TrianglesCount;
		size_t LinesCount;

		std::vector<Vector3> CubesRelativePositions;
	};

	static void LoadShape(std::istream& stream, ShapeData& shapeData);
	static Shape* NewShape(const ShapeData& shapeData);

	typedef std::vector<ShapeData> ShapesDataContainer;
	static ShapesDataContainer m_Shapes;
};
