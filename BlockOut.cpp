#include "pch.h"
#include "BlockOut.h"

#include "Box.h"
#include "Grid.h"
#include "LevelPole.h"
#include "ShapeFactory.h"
#include "Shape.h"

using namespace std;
using namespace nsc;

const Vector3 SHAPE_INITIAL_POSITION_IN_GRID(0.0f, 0.0f, 6.6f);
const char* HIGH_SCORE_FILE_NAME = "score.txt";
const int LAST_LEVEL = 9;
const float LEVEL_TIME_INTERVAL = 60.0f;

void DrawText(int x, int y, const string& text, ID3DX10Font* pFont, const Color& color = WHITE)
{
	RECT rect = {x, y, 0, 0};
	pFont->DrawText(0, text.c_str(), -1, &rect, DT_NOCLIP, color);
}

float ComputeFallingTimeForLevel(int level)
{
	assert(level <= LAST_LEVEL);
	return (LAST_LEVEL - level) * 0.1f;
}

BlockOut::BlockOut(HINSTANCE hInstance)
	: D3DApplication(hInstance)
	, m_pEffect(nullptr)
	, m_pEffectViewProjection(nullptr)
	, m_pVertexLayout(nullptr)
	, m_pTransparentBS(nullptr)
	, m_pFont(nullptr)
	, m_pGrid(nullptr)
	, m_pLevelPole(nullptr)
	, m_pCurrentShape(nullptr)
	, m_pNextShape(nullptr)
	, m_Level(0)
	, m_PlayedCubesCount(0)
	, m_Score(0)
	, m_HighScore(0)
	, m_LastKeyPressed(0) 
	, m_NextLevelStartTime(LEVEL_TIME_INTERVAL)
	, m_ShapeFallingTime(ComputeFallingTimeForLevel(0))
	, m_CurrentTimeAfterLastFall(0.0f)
	, m_IsGamePaused(false)
	, m_IsGameOver(false)
{
	D3DXMatrixIdentity(&m_View);
	D3DXMatrixIdentity(&m_Projection);
}

BlockOut::~BlockOut()
{
	WriteHighScore();

	if (m_pDevice)
	{
		m_pDevice->ClearState();
	}

	Box::ReleaseBuffers();
	ShapeFactory::ReleaseBuffers();

	SafeRelease(m_pEffect);
	SafeRelease(m_pVertexLayout);
	SafeRelease(m_pTransparentBS);
	SafeRelease(m_pFont);

	SafeDelete(m_pGrid);
	SafeDelete(m_pLevelPole);
	SafeDelete(m_pCurrentShape);
	SafeDelete(m_pNextShape);
}

void BlockOut::InitApplication()
{
	__super::InitApplication();

	srand((unsigned)time(nullptr));

	BuildEffect();
	BuildVertexLayout();
	BuildBlendStates();
	BuildDepthStencilState();
	BuildFont();

	ReadHighScore();

	GameObject::InitializeRenderingParameters(m_pDevice, m_pEffect);
	Box::Initialize();

	// set scene
	m_pGrid = Grid::Create();
	m_pGrid->SetPosition(-2.5f, -2.5f, 6.1f);

	m_pLevelPole = LevelPole::Create();
	m_pLevelPole->SetScale(0.4f, 0.4f, 0.4f);
	m_pLevelPole->SetPosition(-3.15f, -2.35f, 6.1f);

	SetCurrentAndNextShapes();

	// Build the view matrix.
	Vector3 position(0.0f, 0.0f, 0.0f);
	Vector3 target(0.0f, 0.0f, 1.0f);
	Vector3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&m_View, &position, &target, &up);

	Matrix VP = m_View * m_Projection;
	m_pEffectViewProjection->SetMatrix((float*)&VP);
}

void BlockOut::OnResize()
{
	__super::OnResize();

	// Recalculate the projection matrix
	float aspectRatio = float(m_ClientWidth) / m_ClientHeight;
	D3DXMatrixPerspectiveFovLH(&m_Projection, 0.25f * PI, aspectRatio, 1.0f, 1000.0f);
}

void BlockOut::UpdateScene( float deltaTime )
{
	__super::UpdateScene(deltaTime);

	switch (m_LastKeyPressed)
	{
	case VK_RETURN:
		if (m_IsGameOver)
		{
			NewGame();
		}
		break;
	case VK_ESCAPE:
		::PostQuitMessage(0);
		break;
	case 'P':
	case VK_PAUSE:
		m_IsGamePaused = !m_IsGamePaused;
		m_IsGamePaused ? m_GameTimer.Stop() : m_GameTimer.Start();
		m_LastKeyPressed = 0;
		break;
	}

	if (m_IsGameOver || m_IsGamePaused)
	{
		return;
	}

	m_CurrentTimeAfterLastFall += deltaTime;

	if (!m_pCurrentShape->IsAnimationStarted())
	{
		if (m_LastKeyPressed != 0)
		{
			switch (m_LastKeyPressed)
			{
			case VK_LEFT:
				m_pCurrentShape->TryToTranslate(-1, 0, 0);
				break;
			case VK_RIGHT:
				m_pCurrentShape->TryToTranslate(1, 0, 0);
				break;
			case VK_UP:
				m_pCurrentShape->TryToTranslate(0, 1, 0);
				break;
			case VK_DOWN:
				m_pCurrentShape->TryToTranslate(0, -1, 0);
				break;
			case VK_SPACE:
				MoveDownCurrentShape();
				break;
			case 'A':
				m_pCurrentShape->TryToRotate(-PI_HALF, 0, 0);
				break;
			case 'Q':
				m_pCurrentShape->TryToRotate(PI_HALF, 0, 0);
				break;
			case 'S':
				m_pCurrentShape->TryToRotate(0, -PI_HALF, 0);
				break;
			case 'W':
				m_pCurrentShape->TryToRotate(0, PI_HALF, 0);
				break;
			case 'D':
				m_pCurrentShape->TryToRotate(0, 0, -PI_HALF);
				break;
			case 'E':
				m_pCurrentShape->TryToRotate(0, 0, PI_HALF);
				break;
			}

			if (m_LastKeyPressed != VK_SPACE)
			{
				m_LastKeyPressed = 0;
			}
		}
		else if (m_ShapeFallingTime <= m_CurrentTimeAfterLastFall)
		{
			m_CurrentTimeAfterLastFall = 0.0f;
			MoveDownCurrentShape();
		}
	}

	if (m_GameTimer.GetGameTime() > m_NextLevelStartTime && m_Level < LAST_LEVEL)
	{
		++m_Level;
		m_NextLevelStartTime += LEVEL_TIME_INTERVAL;
		m_ShapeFallingTime = ComputeFallingTimeForLevel(m_Level);
	}

	m_pCurrentShape->Update(deltaTime);
	m_pNextShape->RotateY(deltaTime);
	m_pLevelPole->Update(m_pCurrentShape->GetShapeHeightInGrid());
}

void BlockOut::DrawScene()
{
	__super::DrawScene();

	// Restore default states and vertex layout
	m_pDevice->OMSetDepthStencilState(m_pDepthStencilState, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	m_pDevice->OMSetBlendState(m_pTransparentBS, blendFactors, 0xffffffff);
	m_pDevice->IASetInputLayout(m_pVertexLayout);

	m_pGrid->Draw();
	m_pLevelPole->Draw();
	
	if (!m_IsGamePaused && !m_IsGameOver)
	{
		m_pGrid->DrawBoxes();
		m_pCurrentShape->Draw();
		m_pNextShape->Draw();
	}
	else if (m_IsGamePaused)
	{
		m_pNextShape->Draw();

		DrawText(m_ClientWidth / 2 - 30, m_ClientHeight / 2 - 10, "PAUSE", m_pFont, RED);
	}
	else if (m_IsGameOver)
	{
		m_pGrid->DrawBoxes();

		DrawText(m_ClientWidth / 2 - 60, m_ClientHeight / 2 - 20, "GAME OVER", m_pFont, WHITE);
		DrawText(m_ClientWidth / 2 - 130, m_ClientHeight / 2, "Press ENTER to start new game", m_pFont, WHITE);
	}
	
	DrawGameInfo();

	m_pSwapChain->Present(0, 0);
}

void BlockOut::BuildEffect()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#ifdef _DEBUG
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
 
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = D3DX10CreateEffectFromFile("BlockOut.fx", 0, 0, 
		"fx_4_0", shaderFlags, 0, m_pDevice, 0, 0, &m_pEffect, &compilationErrors, 0);
	if (FAILED(hr))
	{
		if (compilationErrors)
		{
			MessageBox(nullptr, (char*)compilationErrors->GetBufferPointer(), "Error", MB_ICONERROR);
			SafeRelease(compilationErrors);
		}
#ifdef _DEBUG
		DXTrace(__FILE__, (DWORD)__LINE__, hr, "D3DX10CreateEffectFromFile", true);
#endif
	}

	m_pEffectViewProjection = m_pEffect->GetVariableByName("g_ViewProjection")->AsMatrix();
}

void BlockOut::BuildVertexLayout()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDescription[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
	};

	// Create the input layout
	D3D10_PASS_DESC passDescription;
	m_pEffect->GetTechniqueByName("BlockOutTechnique")->GetPassByIndex(0)->GetDesc(&passDescription);
	HR(m_pDevice->CreateInputLayout(vertexDescription, 1, passDescription.pIAInputSignature,
		passDescription.IAInputSignatureSize, &m_pVertexLayout));
}

void BlockOut::BuildBlendStates()
{
	D3D10_BLEND_DESC blendDescription;
	::ZeroMemory(&blendDescription, sizeof(blendDescription));

	blendDescription.AlphaToCoverageEnable = false;
	blendDescription.BlendEnable[0] = true;
	blendDescription.SrcBlend = D3D10_BLEND_SRC_ALPHA;
	blendDescription.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
	blendDescription.BlendOp = D3D10_BLEND_OP_ADD;
	blendDescription.SrcBlendAlpha = D3D10_BLEND_ONE;
	blendDescription.DestBlendAlpha = D3D10_BLEND_ZERO;
	blendDescription.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	blendDescription.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
	
	HR(m_pDevice->CreateBlendState(&blendDescription, &m_pTransparentBS));
}

void BlockOut::BuildDepthStencilState()
{
	D3D10_DEPTH_STENCIL_DESC depthStencilDescription;
	::ZeroMemory(&depthStencilDescription, sizeof(depthStencilDescription));

	depthStencilDescription.DepthEnable = true;
	depthStencilDescription.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	depthStencilDescription.DepthFunc = D3D10_COMPARISON_LESS_EQUAL;

	HR(m_pDevice->CreateDepthStencilState(&depthStencilDescription, &m_pDepthStencilState));
}

void BlockOut::BuildFont()
{
	D3DX10_FONT_DESC fontDescription;
	fontDescription.Height			 = 24;
	fontDescription.Width			= 0;
	fontDescription.Weight			= 0;
	fontDescription.MipLevels		= 1;
	fontDescription.Italic			= false;
	fontDescription.CharSet			= DEFAULT_CHARSET;
	fontDescription.OutputPrecision	= OUT_DEFAULT_PRECIS;
	fontDescription.Quality			= DEFAULT_QUALITY;
	fontDescription.PitchAndFamily	= DEFAULT_PITCH | FF_DONTCARE;
	strcpy(fontDescription.FaceName, "Times New Roman");

	D3DX10CreateFontIndirect(m_pDevice, &fontDescription, &m_pFont);
}

void BlockOut::NewGame()
{
	m_Level = 0;
	m_PlayedCubesCount = 0;
	m_Score = 0;
	m_CurrentTimeAfterLastFall = 0.0f;
	m_NextLevelStartTime = LEVEL_TIME_INTERVAL;
	m_ShapeFallingTime = ComputeFallingTimeForLevel(0);
	m_LastKeyPressed = 0;
	m_IsGameOver = false;
	m_pGrid->DeleteBoxes();
	SetCurrentAndNextShapes();
	m_GameTimer.Reset();
}

void BlockOut::GameOver()
{
	m_GameTimer.Stop();
	m_IsGameOver = true;
}

void BlockOut::ReadHighScore()
{
	ifstream file(HIGH_SCORE_FILE_NAME);

	if (!file.fail())
	{
		file >> m_HighScore;
	}
}

void BlockOut::WriteHighScore() const
{
	ofstream file(HIGH_SCORE_FILE_NAME);
	file << m_HighScore;
}

void BlockOut::SetCurrentAndNextShapes()
{
	ShapeFactory::LoadShapeSetFromFile("FlatFun.txt");
	m_pCurrentShape = ShapeFactory::CreateRandomShape();
	m_pCurrentShape->SetPosition(SHAPE_INITIAL_POSITION_IN_GRID);
	SetNextShapePreview();
}

void BlockOut::SetNextShapePreview()
{
	m_pNextShape = ShapeFactory::CreateRandomShape();
	
	m_pNextShape->SetScale(0.20f, 0.20f, 0.20f);
	m_pNextShape->RotateZ(PI / 2);
	m_pNextShape->SetPosition(2.9f, 0.9f, 6.0f);
}

void BlockOut::DrawGameInfo() const
{
	// current level
	DrawText(705, 20, "LEVEL: " + NumberToString(m_Level), m_pFont);

	// next shape
	DrawText(705, 80, "NEXT", m_pFont);
	DrawText(705, 100, "SHAPE:", m_pFont);

	// cubes played
	DrawText(705, 250, "CUBES", m_pFont);
	DrawText(705, 270, "PLAYED:", m_pFont);
	DrawText(705, 300, NumberToString(m_PlayedCubesCount), m_pFont);

	// score
	DrawText(705, 350, "SCORE:", m_pFont);
	DrawText(705, 380, NumberToString(m_Score), m_pFont);

	// high score
	DrawText(705, 430, "HIGH", m_pFont);
	DrawText(705, 450, "SCORE:", m_pFont);
	DrawText(705, 480, NumberToString(m_HighScore), m_pFont);
}

void BlockOut::MoveDownCurrentShape()
{
	if (!m_pCurrentShape->TryToTranslate(0, 0, 1))
	{
		m_PlayedCubesCount += m_pCurrentShape->GetCompoundedBlocksCount();
		m_pCurrentShape->Destroy();
		m_Score += m_pGrid->UpdateLevels() * (m_Level + 1);

		if (m_Score > m_HighScore)
		{
			m_HighScore = m_Score;
		}

		if (m_pGrid->HasBoxOnHighestLevel())
		{
			GameOver();
		}

		m_pNextShape->SetWorldTransformationToIdentity();
		m_pCurrentShape = m_pNextShape;
		m_pCurrentShape->SetPosition(SHAPE_INITIAL_POSITION_IN_GRID);
		SetNextShapePreview();

		m_LastKeyPressed = 0;
		m_CurrentTimeAfterLastFall = 0.0f;
	}
}

void BlockOut::OnKeyPressed(unsigned key)
{
	m_LastKeyPressed = key;
}
