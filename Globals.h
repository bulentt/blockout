#pragma once

typedef D3DXVECTOR3 Vector3;
typedef D3DXQUATERNION Quaternion;
typedef D3DXMATRIX Matrix;

typedef D3DXCOLOR Color;

const float PI =	3.14159265358979323846f;
const float E =		2.71828182845904523536f;

const float PI_HALF = 0.5f * PI;

inline std::istream& operator >> (std::istream& str, Vector3& v)
{
	str >> v.x >> v.y >> v.z;
	return str;
}

inline bool IsZero(float value)
{
	return fabs(value) < std::numeric_limits<float>::epsilon();
}

inline bool IsInRange(float value, float left, float right)
{
	return left <= value && value <= right;
}

inline int Round(float value)
{
	return (value < 0.0f) ? int(value - 0.5f) : int(value + 0.5f);
}

inline float Clamp(float value, float min, float max)
{
	if (value < min)
	{
		return min;
	}

	if (value > max)
	{
		return max;
	}

	return value;
}
