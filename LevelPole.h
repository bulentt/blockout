#pragma once

#include "GameObject.h"

class LevelPole : public GameObject
{
public:
	static LevelPole* Create(const Color& color = GREEN);
	static LevelPole* GetInstance();

	virtual void Draw() const;

	void Update(size_t shapeHeight);

	virtual ~LevelPole();

private:
	LevelPole(const Color& color);

	void DrawLevels() const;

	size_t m_ShapeCurrentHeight;

	static LevelPole* m_pInstance;
	static const size_t HEIGHT = 12;

	ID3D10Buffer* m_pVertexBuffer;
	ID3D10Buffer* m_pIndexBuffer;

	size_t m_IndicesCount;
};
