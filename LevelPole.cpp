#include "pch.h"
#include "LevelPole.h"
#include "Grid.h"

using namespace std;

LevelPole* LevelPole::Create(const Color& color /* = GREEN */)
{
	assert(!m_pInstance);
	return m_pInstance = new LevelPole(color);
}

LevelPole* LevelPole::GetInstance()
{
	assert(m_pInstance);
	return m_pInstance;
}

void LevelPole::Draw() const
{
	SetWorldTransformation();
	SetVertexBuffer(m_pVertexBuffer);
	DrawLevels();
	DrawLines(m_pIndexBuffer, m_IndicesCount / 2, m_Color);
}

void LevelPole::Update(size_t shapeHeight)
{
	m_ShapeCurrentHeight = shapeHeight;
}

LevelPole::~LevelPole()
{
	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pIndexBuffer);
}

LevelPole::LevelPole(const Color& color)
	: GameObject(color)
	, m_pVertexBuffer(nullptr)
	, m_pIndexBuffer(nullptr)
	, m_IndicesCount(0)
{
	//::ZeroMemory(&m_HasBoxInLevel, HEIGHT);

	vector<Vertex> vertices;
	vector<unsigned> indices;
	
	for (size_t i = 0; i <= HEIGHT; ++i)
	{
		vertices.push_back(Vertex(1.0f, float(i), 0.0f));
		vertices.push_back(Vertex(0.0f, float(i), 0.0f));

		indices.push_back(2 * i);
		indices.push_back(2 * i + 1);
	}

	indices.push_back(0);
	indices.push_back(HEIGHT * 2);
	indices.push_back(1);
	indices.push_back(HEIGHT * 2 + 1);

	m_IndicesCount = indices.size();

	CreateVertexBuffer(m_pVertexBuffer, &vertices.front(), vertices.size());
	CreateIndexBuffer(m_pIndexBuffer, &indices.front(), m_IndicesCount);
}

void LevelPole::DrawLevels() const
{
	Grid* pGrid = Grid::GetInstance();
	m_pDevice->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	m_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (size_t i = 0; i < HEIGHT; ++i)
	{
		if (i >= HEIGHT - pGrid->GetHighestLevelWithBox())
		{
			break;
		}

		m_pEffectColor->SetFloatVector((float*)&Grid::GetLevelColor(HEIGHT - i - 1));
		m_pTechnique->GetPassByIndex(0)->Apply(0);
		m_pDevice->DrawIndexed(4, i, i);
	}

	if (!pGrid->HasBoxOnHighestLevel())
	{
		m_pEffectColor->SetFloatVector((float*)&WHITE);
		m_pTechnique->GetPassByIndex(0)->Apply(0);
		m_pDevice->DrawIndexed(4, HEIGHT - m_ShapeCurrentHeight - 1, HEIGHT - m_ShapeCurrentHeight - 1);
	}
}

LevelPole* LevelPole::m_pInstance = nullptr;
