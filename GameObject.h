#pragma once

class GameObject
{
public:
	GameObject(const Color& color = BLACK);
	virtual ~GameObject() {}

	const Matrix& GetWorldMatrix() const;

	void SetWorldTransformationToIdentity();
	
	const Vector3&		GetScale()		const; 
	const Quaternion&	GetRotation()	const;
	const Vector3&		GetPosition()	const;

#pragma region rendering

public:
	virtual void Draw() const = 0;

	static void InitializeRenderingParameters(ID3D10Device* pDevice, ID3D10Effect* pEffect);

	void SetColor(const Color& color);

protected:
	void SetWorldTransformation() const;
	static void SetWorldTransformation(const Matrix& world);

	static void CreateVertexBuffer(ID3D10Buffer*& pVertexBuffer, Vertex* pVertices, size_t count);
	static void CreateIndexBuffer(ID3D10Buffer*& pIndexBuffer, unsigned* pIndices, size_t count);

	static void SetVertexBuffer(ID3D10Buffer* pVertexBuffer);

	static void DrawTriangles(ID3D10Buffer* pBuffer, size_t trianglesCount, const Color& color);
	static void DrawLines(ID3D10Buffer* pBuffer, size_t linesCount, const Color& color);

	static ID3D10Device*				m_pDevice;
	static ID3D10EffectTechnique*		m_pTechnique;
	static ID3D10EffectMatrixVariable*	m_pEffectWorld;
	static ID3D10EffectVectorVariable*	m_pEffectColor;

	Color m_Color;

private:

	template <typename T>
	static void CreateBuffer(ID3D10Buffer*& pBuffer, T* pData, size_t count, D3D10_BIND_FLAG flag); 

	static void DrawIndexed(ID3D10Buffer* pBuffer, size_t count, D3D10_PRIMITIVE_TOPOLOGY topology, const Color& color);

#pragma endregion

public:

#pragma region scale modifiers
	
	void ScaleX(float x);
	void ScaleY(float y);
	void ScaleZ(float z);

	void Scale(float x, float y, float z);
	void Scale(const Vector3& scale);

	void SetScale(float x, float y, float z);
	void SetScale(const Vector3& scale);

#pragma endregion
	
#pragma region rotation modifiers

	void RotateX(float angle);
	void RotateY(float angle);
	void RotateZ(float angle);
		
	void Rotate(float x, float y, float z);
	void Rotate(const Quaternion& rotation);
	void Rotate(const Matrix& rotation);
	void Rotate(const Vector3& axes, float angle);
	
	void SetRotation(float x, float y, float z);
	void SetRotation(const Quaternion& rotation);
	void SetRotation(const Matrix& rotation);
	void SetRotation(const Vector3& axes, float angle);

#pragma endregion

#pragma region position modifiers

	void TranslateX(float step);
	void TranslateY(float step);
	void TranslateZ(float step);

	void Translate(float x, float y, float z);
	void Translate(const Vector3& translation);

	void SetPosition(float x, float y, float z);
	void SetPosition(const Vector3& position);

#pragma endregion

private:

	// world matrix components
	Vector3		m_Scale;
	Quaternion	m_Rotation;
	Vector3		m_Position;

	mutable Matrix	m_World;
	mutable bool	m_IsWorldMatrixChanged;
};
