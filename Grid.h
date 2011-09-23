#pragma once

#include "GameObject.h"

class Box;

class Grid : public GameObject
{
public:
	static Grid* Create(const Color& color = GREEN);
	static Grid* GetInstance();

	virtual void Draw() const;
	virtual ~Grid();

	void DrawBoxes() const;
	void DeleteBoxes();

	// returns trucated levels count
	unsigned UpdateLevels();

	bool HasBoxOn(size_t x, size_t y, size_t z) const;
	void SetBoxOn(size_t x, size_t y, size_t z);

	size_t GetHighestLevelWithBox() const;
	bool HasBoxOnHighestLevel() const;

	static const size_t X_SIZE = 5;
	static const size_t Y_SIZE = 5;
	static const size_t Z_SIZE = 12;

	static const Color& GetLevelColor(unsigned level);

private:
	Grid(const Color& color);

	void TruncateLevel(int level);
	bool IsLevelFull(int level) const;

	size_t m_HighestLevelWithBox;

	static Grid* m_pInstance;

	ID3D10Buffer* m_pVertexBuffer;
	ID3D10Buffer* m_pIndexBuffer;

	size_t m_IndicesCount;

	Box* m_Boxes[Z_SIZE][Y_SIZE][X_SIZE];

	static const size_t LEVELS_COLORS_COUNT = 6;
	static const Color LEVELS_COLORS[LEVELS_COLORS_COUNT];
};
