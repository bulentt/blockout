#include "pch.h"
#include "Shape.h"
#include "Grid.h"

void Shape::Draw() const
{
	SetWorldTransformation();
	SetVertexBuffer(m_pVertexBuffer);
	// draw border
	DrawLines(m_pLineIndexBuffer, m_LinesCount, WHITE);
	// draw shape
	DrawTriangles(m_pTriangleIndexBuffer, m_TrianglesCount, m_Color);
}

void Shape::Update(float time)
{
	if (m_IsAnimationStarted)
	{
		m_CurrentTimeFromStartOfAnimation += time;
		m_InterpolationRatio = Clamp(m_CurrentTimeFromStartOfAnimation / m_AnimationTime, 0.0f, 1.0f);

		switch (m_AnimationType)
		{
		case TRANSLATION:
			{
				Vector3 position;
				D3DXVec3Lerp(&position, &m_StartPosition, &m_FinalPosition, m_InterpolationRatio);
				SetPosition(position);
			}
			break;
		case ROTATION:
			{
				Quaternion rotation;
				D3DXQuaternionSlerp(&rotation, &m_StartRotation, &m_FinalRotation, m_InterpolationRatio);
				SetRotation(rotation);
			}
			break;
		default:
			// must not enter here
			assert(0);
		}

		if (m_CurrentTimeFromStartOfAnimation >= m_AnimationTime)
		{
			m_IsAnimationStarted = false;
		}
	}
}

void Shape::Destroy()
{
	Grid* pGrid = Grid::GetInstance();

	CubesContainer::const_iterator it = m_CubesRelativePositions.begin();
	for ( ; it != m_CubesRelativePositions.end(); ++it)
	{
		Vector3 newCubePosition = m_PositionInGrid + *it;
		
		if (newCubePosition.z >= 0)
		{
			pGrid->SetBoxOn(size_t(newCubePosition.x), size_t(newCubePosition.y), size_t(newCubePosition.z));
		}
	}

	delete this;
}

bool Shape::IsAnimationStarted() const
{
	return m_IsAnimationStarted;
}

size_t Shape::GetShapeHeightInGrid() const
{
	int maxHeight = int(m_PositionInGrid.z);

	CubesContainer::const_iterator it = m_CubesRelativePositions.begin();
	for ( ; it != m_CubesRelativePositions.end(); ++it)
	{
		int cubeHeight = int(m_PositionInGrid.z + it->z);

		if (maxHeight < cubeHeight)
		{
			maxHeight = cubeHeight;
		}
	}

	return size_t(maxHeight);
}

size_t Shape::GetCompoundedBlocksCount() const
{
	return m_CubesRelativePositions.size();
}

bool Shape::TryToTranslate(float x, float y, float z)
{
	Vector3 translation(x, y, z);

	m_PositionInGrid += translation;
	
	if (!IsMovePosible())
	{
		m_PositionInGrid -= translation;
		return false;
	}
	else
	{
		StartToTranslate(translation, ANIMATION_TIME_DURATION);
		return true;
	}
}

bool Shape::TryToRotate(float x, float y, float z)
{
	Matrix rotation;
	D3DXMatrixRotationYawPitchRoll(&rotation, y, x, z);

	CubesContainer oldCubesRelativePositions = m_CubesRelativePositions;

	CubesContainer::iterator it = m_CubesRelativePositions.begin();
	for ( ; it != m_CubesRelativePositions.end(); ++it)
	{
		D3DXVec3TransformCoord(&*it, &*it, &rotation);

		it->x = float(Round(it->x));
		it->y = float(Round(it->y));
		it->z = float(Round(it->z));
	}

	if (!IsMovePosible())
	{
		m_CubesRelativePositions = oldCubesRelativePositions;
		return false;
	}
	else
	{
		Quaternion qu;
		D3DXQuaternionRotationMatrix(&qu, &rotation);
		StartToRotate(qu, ANIMATION_TIME_DURATION);
		return true;
	}
}

Shape::Shape(	
	ID3D10Buffer* pVertexBuffer,
	ID3D10Buffer* pTriangleIndexBuffer,
	ID3D10Buffer* pLineIndexBuffer,
	size_t trianglesCount,
	size_t linesCount,
	const CubesContainer& cubes,
	const Color& color /*= Color(1.0f, 1.0f, 1.0f, 0.25f*/
)
		: GameObject(color)
		, m_pVertexBuffer(pVertexBuffer)
		, m_pTriangleIndexBuffer(pTriangleIndexBuffer)
		, m_pLineIndexBuffer(pLineIndexBuffer)
		, m_TrianglesCount(trianglesCount)
		, m_LinesCount(linesCount)
		, m_CubesRelativePositions(cubes)
		, m_IsAnimationStarted(false)
		, m_CurrentTimeFromStartOfAnimation(0.0f)
		, m_PositionInGrid(float(Grid::X_SIZE / 2), float(Grid::Y_SIZE / 2), 0.0f)
{
}

bool Shape::IsMovePosible() const
{
	Grid* pGrid = Grid::GetInstance();

	CubesContainer::const_iterator it = m_CubesRelativePositions.begin();
	for ( ; it != m_CubesRelativePositions.end(); ++it)
	{
		Vector3 cubePosition = m_PositionInGrid + *it;

		if (// out of grid
			(!IsInRange(cubePosition.x, 0, Grid::X_SIZE - 1) ||
			 !IsInRange(cubePosition.y, 0, Grid::Y_SIZE - 1) ||
			 cubePosition.z > Grid::Z_SIZE - 1)
			|| //or already has box on this position
			(pGrid->HasBoxOn(size_t(cubePosition.x), size_t(cubePosition.y), size_t(cubePosition.z))))
		{
			return false;
		}
	}

	return true;
}

void Shape::StartToTranslate( const Vector3& translation, float animationTime )
{
	m_AnimationTime = animationTime;
	m_StartPosition = GetPosition();
	m_FinalPosition = m_StartPosition + translation;
	m_InterpolationRatio = 0.0f;
	m_CurrentTimeFromStartOfAnimation = 0.0f;
	m_AnimationType = TRANSLATION;
	m_IsAnimationStarted = true;
}

void Shape::StartToRotate( const Quaternion& rotation, float animationTime )
{
	m_AnimationTime = animationTime;
	m_StartRotation = GetRotation();
	m_FinalRotation = m_StartRotation * rotation;
	m_InterpolationRatio = 0.0f;
	m_CurrentTimeFromStartOfAnimation = 0.0f;
	m_AnimationType = ROTATION;
	m_IsAnimationStarted = true;
}

const float Shape::ANIMATION_TIME_DURATION = 0.1f;
