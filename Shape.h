#pragma once

#include "GameObject.h"

class Shape : public GameObject
{
	friend class ShapeFactory;

public:
	virtual void Draw() const;

	void Update(float time);
	void Destroy();

	bool IsAnimationStarted() const;

	size_t GetShapeHeightInGrid() const;
	size_t GetCompoundedBlocksCount() const;

	bool TryToTranslate(float x, float y, float z);
	bool TryToRotate(float x, float y, float z);

private:

	typedef std::vector<Vector3> CubesContainer;

	Shape(ID3D10Buffer* pVertexBuffer,
		  ID3D10Buffer* pTriangleIndexBuffer,
		  ID3D10Buffer* pLineIndexBuffer,
		  size_t trianglesCount,
		  size_t linesCount,
		  const CubesContainer& cubes,
		  const Color& color = Color(1.0f, 1.0f, 1.0f, 0.25f));

	bool IsMovePosible() const;

	ID3D10Buffer* m_pVertexBuffer;
	ID3D10Buffer* m_pTriangleIndexBuffer;
	ID3D10Buffer* m_pLineIndexBuffer;

	size_t m_TrianglesCount;
	size_t m_LinesCount;

	Vector3 m_PositionInGrid;
	CubesContainer m_CubesRelativePositions;

#pragma region animation

	void StartToTranslate(const Vector3& translation, float animationTime);
	void StartToRotate(const Quaternion& rotation, float animationTime);

	enum AnimationType
	{
		TRANSLATION,
		ROTATION,
	} m_AnimationType;

	bool m_IsAnimationStarted;

	float m_InterpolationRatio;

	Vector3 m_StartPosition;
	Vector3 m_FinalPosition;

	Quaternion m_StartRotation;
	Quaternion m_FinalRotation;

	float m_CurrentTimeFromStartOfAnimation; // in seconds
	float m_AnimationTime; // in seconds

	static const float ANIMATION_TIME_DURATION;

#pragma endregion

};
