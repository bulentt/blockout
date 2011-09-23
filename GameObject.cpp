#include "pch.h"
#include "GameObject.h"

GameObject::GameObject(const Color& color /*= BLACK*/)
	: m_IsWorldMatrixChanged(false)
	, m_Scale(1.0f, 1.0f, 1.0f)
	, m_Position(0.0f, 0.0f, 0.0f)
	, m_Color(color)
{
	D3DXQuaternionIdentity(&m_Rotation);
	D3DXMatrixIdentity(&m_World);
}

const Matrix& GameObject::GetWorldMatrix() const
{
	if (m_IsWorldMatrixChanged)
	{
		Matrix scale, rotation, translation;
		
		D3DXMatrixScaling(&scale, m_Scale.x, m_Scale.y, m_Scale.z);
		D3DXMatrixRotationQuaternion(&rotation, &m_Rotation);
		D3DXMatrixTranslation(&translation, m_Position.x, m_Position.y, m_Position.z);

		m_World = scale * rotation * translation;

		m_IsWorldMatrixChanged = false;
	}

	return m_World;
}

void GameObject::SetWorldTransformationToIdentity()
{
	m_Position = Vector3(0.0f, 0.0f, 0.0f);
	m_Scale = Vector3(1.0f, 1.0f, 1.0f);

	D3DXQuaternionIdentity(&m_Rotation);
	D3DXMatrixIdentity(&m_World);

	m_IsWorldMatrixChanged = false;
}

const Vector3& GameObject::GetScale() const
{
	return m_Scale;
}

const Quaternion& GameObject::GetRotation() const
{
	return m_Rotation;
}

const Vector3& GameObject::GetPosition() const
{
	return m_Position;
}

// rendering related

void GameObject::InitializeRenderingParameters( ID3D10Device* pDevice, ID3D10Effect* pEffect )
{
	m_pDevice = pDevice;
	m_pTechnique = pEffect->GetTechniqueByName("BlockOutTechnique");
	m_pEffectWorld = pEffect->GetVariableByName("g_World")->AsMatrix();
	m_pEffectColor = pEffect->GetVariableByName("g_Color")->AsVector();
}

void GameObject::SetColor(const Color& color)
{
	m_Color = color;
}

void GameObject::SetWorldTransformation() const
{
	m_pEffectWorld->SetMatrix((float*)&GetWorldMatrix());
}

void GameObject::SetWorldTransformation( const Matrix& world )
{
	m_pEffectWorld->SetMatrix((float*)&world);
}

void GameObject::CreateVertexBuffer( ID3D10Buffer*& pVertexBuffer, Vertex* pVertices, size_t count )
{
	CreateBuffer(pVertexBuffer, pVertices, count, D3D10_BIND_VERTEX_BUFFER);
}

void GameObject::CreateIndexBuffer( ID3D10Buffer*& pIndexBuffer, unsigned* pIndices, size_t count )
{
	CreateBuffer(pIndexBuffer, pIndices, count, D3D10_BIND_INDEX_BUFFER);
}

void GameObject::SetVertexBuffer( ID3D10Buffer* pVertexBuffer )
{
	unsigned stride = sizeof(Vertex);
	unsigned offset = 0;
	m_pDevice->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
}

void GameObject::DrawTriangles(ID3D10Buffer* pBuffer, size_t trianglesCount, const Color& color)
{
	DrawIndexed(pBuffer, trianglesCount * 3, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, color);
}

void GameObject::DrawLines(ID3D10Buffer* pBuffer, size_t linesCount, const Color& color)
{
	DrawIndexed(pBuffer, linesCount * 2, D3D10_PRIMITIVE_TOPOLOGY_LINELIST, color);
}

template <typename T>
void GameObject::CreateBuffer(  ID3D10Buffer*& pBuffer, T* pData, size_t count, D3D10_BIND_FLAG flag)
{
	D3D10_BUFFER_DESC bufferDescription;
	bufferDescription.Usage = D3D10_USAGE_IMMUTABLE;
	bufferDescription.ByteWidth = sizeof(T) * count;
	bufferDescription.BindFlags = flag;
	bufferDescription.CPUAccessFlags = 0;
	bufferDescription.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA initData;
	initData.pSysMem = pData;

	HR(m_pDevice->CreateBuffer(&bufferDescription, &initData, &pBuffer));
}

void GameObject::DrawIndexed( ID3D10Buffer* pBuffer, size_t count, D3D10_PRIMITIVE_TOPOLOGY topology, const Color& color )
{
	m_pDevice->IASetPrimitiveTopology(topology);
	m_pDevice->IASetIndexBuffer(pBuffer, DXGI_FORMAT_R32_UINT, 0);
	m_pEffectColor->SetFloatVector((float*)&color);
	m_pTechnique->GetPassByIndex(0)->Apply(0);
	m_pDevice->DrawIndexed(count, 0, 0);
}

// static members initialization

ID3D10Device*				GameObject::m_pDevice		= nullptr;
ID3D10EffectTechnique*		GameObject::m_pTechnique	= nullptr;
ID3D10EffectMatrixVariable* GameObject::m_pEffectWorld	= nullptr;
ID3D10EffectVectorVariable* GameObject::m_pEffectColor	= nullptr;

// scale modifiers

void GameObject::ScaleX( float x )
{
	assert(!IsZero(x));
	m_Scale.x *= x;
	m_IsWorldMatrixChanged = true;
}

void GameObject::ScaleY( float y )
{
	assert(!IsZero(y));
	m_Scale.y *= y;
	m_IsWorldMatrixChanged = true;
}

void GameObject::ScaleZ( float z )
{
	assert(!IsZero(z));
	m_Scale.z *= z;
	m_IsWorldMatrixChanged = true;
}

void GameObject::Scale( float x, float y, float z )
{
	assert(!IsZero(x));
	assert(!IsZero(y));
	assert(!IsZero(z));

	m_Scale.x *= x;
	m_Scale.y *= y;
	m_Scale.z *= z;

	m_IsWorldMatrixChanged = true;
}

void GameObject::Scale( const Vector3& scale )
{
	assert(!IsZero(scale.x));
	assert(!IsZero(scale.y));
	assert(!IsZero(scale.z));

	m_Scale.x *= scale.x;
	m_Scale.y *= scale.y;
	m_Scale.z *= scale.z;

	m_IsWorldMatrixChanged = true;
}

void GameObject::SetScale( float x, float y, float z )
{
	assert(!IsZero(x));
	assert(!IsZero(y));
	assert(!IsZero(z));

	m_Scale = Vector3(x, y, z);

	m_IsWorldMatrixChanged = true;
}

void GameObject::SetScale( const Vector3& scale )
{
	assert(!IsZero(scale.x));
	assert(!IsZero(scale.y));
	assert(!IsZero(scale.z));

	m_Scale = scale;

	m_IsWorldMatrixChanged = true;
}

// rotation modifiers
	
void GameObject::RotateX( float angle )
{
	Quaternion qu;
	D3DXQuaternionRotationYawPitchRoll(&qu, 0, angle, 0);
	m_Rotation *= qu;
	m_IsWorldMatrixChanged = true;
}

void GameObject::RotateY( float angle )
{
	Quaternion qu;
	D3DXQuaternionRotationYawPitchRoll(&qu, angle, 0, 0);
	m_Rotation *= qu;
	m_IsWorldMatrixChanged = true;
}

void GameObject::RotateZ( float angle )
{
	Quaternion qu;
	D3DXQuaternionRotationYawPitchRoll(&qu, 0, 0, angle);
	m_Rotation *= qu;
	m_IsWorldMatrixChanged = true;
}

void GameObject::Rotate( float x, float y, float z )
{
	Quaternion qu;
	D3DXQuaternionRotationYawPitchRoll(&qu, y, x, z);
	m_Rotation *= qu;
	m_IsWorldMatrixChanged = true;
}

void GameObject::Rotate( const Matrix& rotation )
{
	Quaternion qu;
	D3DXQuaternionRotationMatrix(&qu, &rotation);
	m_Rotation *= qu;
	m_IsWorldMatrixChanged = true;
}

void GameObject::Rotate( const Quaternion& rotation )
{
	m_Rotation *= rotation;
	m_IsWorldMatrixChanged = true;
}

void GameObject::Rotate( const Vector3& axes, float angle )
{
	Quaternion qu;
	D3DXQuaternionRotationAxis(&qu, &axes, angle);
	m_Rotation *= qu;
	m_IsWorldMatrixChanged = true;
}

void GameObject::SetRotation( float x, float y, float z )
{
	D3DXQuaternionRotationYawPitchRoll(&m_Rotation, y, x, z);
	m_IsWorldMatrixChanged = true;
}

void GameObject::SetRotation( const Quaternion& rotation )
{
	m_Rotation = rotation;
	m_IsWorldMatrixChanged = true;
}

void GameObject::SetRotation( const Vector3& axes, float angle )
{
	D3DXQuaternionRotationAxis(&m_Rotation, &axes, angle);
	m_IsWorldMatrixChanged = true;
}

void GameObject::SetRotation( const Matrix& rotation )
{
	D3DXQuaternionRotationMatrix(&m_Rotation, &rotation);
	m_IsWorldMatrixChanged = true;
}

// translation modifiers

void GameObject::TranslateX( float step )
{
	m_Position.x += step;
	m_IsWorldMatrixChanged = true;
}

void GameObject::TranslateY( float step )
{
	m_Position.y += step;
	m_IsWorldMatrixChanged = true;
}

void GameObject::TranslateZ( float step )
{
	m_Position.z += step;
	m_IsWorldMatrixChanged = true;
}

void GameObject::Translate( float x, float y, float z )
{
	m_Position += Vector3(x, y, z);
	m_IsWorldMatrixChanged = true;
}

void GameObject::Translate( const Vector3& translation )
{
	m_Position += translation;
	m_IsWorldMatrixChanged = true;
}

void GameObject::SetPosition( float x, float y, float z )
{
	m_Position = Vector3(x, y, z);
	m_IsWorldMatrixChanged = true;
}

void GameObject::SetPosition( const Vector3& position )
{
	m_Position = position;
	m_IsWorldMatrixChanged = true;
}
