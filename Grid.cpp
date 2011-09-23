#include "pch.h"
#include "Grid.h"
#include "Box.h"

using namespace std;

Grid* Grid::Create(const Color& color /* = GREEN */)
{
	assert(!m_pInstance);
	return m_pInstance = new Grid(color);
}

Grid* Grid::GetInstance()
{
	assert(m_pInstance);
	return m_pInstance;
}

void Grid::Draw() const
{	
	SetWorldTransformation();
	SetVertexBuffer(m_pVertexBuffer);
	DrawLines(m_pIndexBuffer, m_IndicesCount / 2, m_Color);
}

Grid::~Grid()
{
	DeleteBoxes();

	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pIndexBuffer);
}

void Grid::DrawBoxes() const
{
	for (int z = Z_SIZE - 1; z >= 0; --z)
	{
		bool hasBoxInLevel = false;

		for (size_t y = 0; y < Y_SIZE; ++y)
		{
			for (size_t x = 0; x < X_SIZE; ++x)
			{
				if (m_Boxes[z][y][x])
				{
					bool isTopVisible = true;
					bool isLeftVisible = true;
					bool isRightVisible = true;
					bool isFrontVisible = true;
					bool isBackVisible = true;

					if (x <= X_SIZE / 2) isLeftVisible = false;
					if (x >= X_SIZE / 2) isRightVisible = false;
					if (y <= Y_SIZE / 2) isFrontVisible = false;
					if (y >= Y_SIZE / 2) isBackVisible = false;
					
					if (HasBoxOn(x - 1, y, z)) isLeftVisible = false;
					if (HasBoxOn(x + 1, y, z)) isRightVisible = false;
					if (HasBoxOn(x, y - 1, z)) isFrontVisible = false;
					if (HasBoxOn(x, y + 1, z)) isBackVisible = false;
					if (HasBoxOn(x, y, z - 1)) isTopVisible = false;
					
					if (isTopVisible || isLeftVisible || isRightVisible || isFrontVisible || isBackVisible)
					{
						m_Boxes[z][y][x]->Draw();
					}

					hasBoxInLevel = true;
				}
			}
		}

		if (!hasBoxInLevel)
		{
			break;
		}
	}
}

void Grid::DeleteBoxes()
{
	for (int z = Z_SIZE - 1; z >= 0; --z)
	{
		bool hasBoxInLevel = false;

		for (size_t y = 0; y < Y_SIZE; ++y)
		{
			for (size_t x = 0; x < X_SIZE; ++x)
			{
				if (m_Boxes[z][y][x])
				{
					SafeDelete(m_Boxes[z][y][x]);
					hasBoxInLevel = true;
				}
			}
		}

		if (!hasBoxInLevel)
		{
			break;
		}
	}

	m_HighestLevelWithBox = Z_SIZE;
}

unsigned Grid::UpdateLevels()
{
	unsigned truncatedLevels = 0;

	for (int level = Z_SIZE - 1; level >= int(m_HighestLevelWithBox); --level)
	{
		if (IsLevelFull(level))
		{
			TruncateLevel(level);
			level = Z_SIZE;
			++truncatedLevels;
		}
	}

	return truncatedLevels;
}

bool Grid::HasBoxOn(size_t x, size_t y, size_t z) const
{
	if (x < 0 || x >= X_SIZE || y < 0 || y >= Y_SIZE || z < 0 || z >= Z_SIZE)
	{
		return false;
	}

	return !!m_Boxes[z][y][x];
}

void Grid::SetBoxOn(size_t x, size_t y, size_t z)
{
	assert(x >= 0 && x < X_SIZE);
	assert(y >= 0 && y < Y_SIZE);
	assert(z >= 0 && z < Z_SIZE);

	assert(!m_Boxes[z][y][x]);

	if (z < m_HighestLevelWithBox)
	{
		m_HighestLevelWithBox = z;
	}

	Box* pBox = new Box(GetLevelColor(z));
	Vector3 boxPositionInGrid((float)x, (float)y, (float)z);
	pBox->SetPosition(GetPosition() + boxPositionInGrid);
	m_Boxes[z][y][x] = pBox;
}

size_t Grid::GetHighestLevelWithBox() const
{
	return m_HighestLevelWithBox;
}

bool Grid::HasBoxOnHighestLevel() const
{
	return GetHighestLevelWithBox() == 0;
}

const Color& Grid::GetLevelColor(unsigned level)
{
	return LEVELS_COLORS[level % LEVELS_COLORS_COUNT];
}

Grid::Grid(const Color& color)
	: GameObject(color)
	, m_pVertexBuffer(nullptr)
	, m_pIndexBuffer(nullptr)
	, m_IndicesCount(0)
	, m_HighestLevelWithBox(Z_SIZE)
{
	::ZeroMemory(m_Boxes, X_SIZE * Y_SIZE * Z_SIZE * sizeof(Box*));

	// GENERATE VERTEX AND INDEX DATA

	vector<Vertex> vertices;
	vector<unsigned> indices;

	// create bottom and top

	for (size_t x = 0; x <= X_SIZE; ++x)
	{
		// bottom
		vertices.push_back(Vertex(float(x), 0.0f, float(Z_SIZE)));
		indices.push_back(vertices.size() - 1);
		vertices.push_back(Vertex(float(x), float(Y_SIZE), float(Z_SIZE)));
		indices.push_back(vertices.size() - 1);

		// top
		vertices.push_back(Vertex(float(x), 0.0f, 0.0f));
		indices.push_back(vertices.size() - 1);
		indices.push_back(vertices.size() - 3);
		vertices.push_back(Vertex(float(x), float(Y_SIZE), 0.0f));
		indices.push_back(vertices.size() - 1);
		indices.push_back(vertices.size() - 3);
	}

	unsigned downLeft = 0;
	unsigned upLeft = 1;
	unsigned downRight = 4 * X_SIZE;
	unsigned upRight = 4 * X_SIZE + 1;

	indices.push_back(downLeft);
	indices.push_back(downRight);
	indices.push_back(upLeft);
	indices.push_back(upRight);

	for (size_t y = 1; y < Y_SIZE; ++y)
	{
		// bottom
		vertices.push_back(Vertex(0.0f, float(y), float(Z_SIZE)));
		indices.push_back(vertices.size() - 1);
		vertices.push_back(Vertex(float(X_SIZE), float(y), float(Z_SIZE)));
		indices.push_back(vertices.size() - 1);

		// top
		vertices.push_back(Vertex(0.0f, float(y), 0.0f));
		indices.push_back(vertices.size() - 1);
		indices.push_back(vertices.size() - 3);
		vertices.push_back(Vertex(float(X_SIZE), float(y), 0.0f));
		indices.push_back(vertices.size() - 1);
		indices.push_back(vertices.size() - 3);
	}

	// create sides

	for (size_t z = Z_SIZE - 1; z > 0; --z)
	{
		// down left
		vertices.push_back(Vertex(0.0f, 0.0f, float(z)));
		indices.push_back(vertices.size() - 1);
		
		// up left
		vertices.push_back(Vertex(0.0f, float(Y_SIZE), float(z)));
		indices.push_back(vertices.size() - 1);
		indices.push_back(vertices.size() - 1);
		
		// up right
		vertices.push_back(Vertex(float(X_SIZE), float(Y_SIZE), float(z)));
		indices.push_back(vertices.size() - 1);
		indices.push_back(vertices.size() - 1);
		
		// down right
		vertices.push_back(Vertex(float(X_SIZE), 0.0f, float(z)));
		indices.push_back(vertices.size() - 1);
		indices.push_back(vertices.size() - 1);
		indices.push_back(vertices.size() - 4);
	}

	// the top floor

	downLeft = 2;
	upLeft = 3;
	downRight = 4 * X_SIZE + 2;
	upRight = 4 * X_SIZE + 3;

	indices.push_back(downLeft);
	indices.push_back(upLeft);
	indices.push_back(upLeft);
	indices.push_back(upRight);
	indices.push_back(upRight);
	indices.push_back(downRight);
	indices.push_back(downRight);
	indices.push_back(downLeft);

	m_IndicesCount = indices.size();

	CreateVertexBuffer(m_pVertexBuffer, &vertices.front(), vertices.size());
	CreateIndexBuffer(m_pIndexBuffer, &indices.front(), m_IndicesCount);
}

void Grid::TruncateLevel(int level)
{
	// delete level
	for (size_t y = 0; y < Y_SIZE; ++y)
	{
		for (size_t x = 0; x < X_SIZE; ++x)
		{
			if (HasBoxOn(x, y, level))
			{
				SafeDelete(m_Boxes[level][y][x]);
			}
		}
	}

	// move down upper levels
	for (size_t z = level - 1; z >= m_HighestLevelWithBox; --z)
	{
		for (size_t y = 0; y < Y_SIZE; ++y)
		{
			for (size_t x = 0; x < X_SIZE; ++x)
			{
				if (HasBoxOn(x, y, z))
				{
					m_Boxes[z][y][x]->Translate(0, 0, 1);
					m_Boxes[z][y][x]->SetColor(GetLevelColor(z + 1));
					m_Boxes[z + 1][y][x] = m_Boxes[z][y][x];
					m_Boxes[z][y][x] = nullptr;
				}
			}
		}
	}

	++m_HighestLevelWithBox;
}

bool Grid::IsLevelFull(int level) const
{
	for (size_t y = 0; y < Y_SIZE; ++y)
	{
		for (size_t x = 0; x < X_SIZE; ++x)
		{
			if (!HasBoxOn(x, y, level))
			{
				return false;
			}
		}
	}

	return true;
}

Grid* Grid::m_pInstance = nullptr;

const Color Grid::LEVELS_COLORS[Grid::LEVELS_COLORS_COUNT] = { RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, };
