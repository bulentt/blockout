#pragma once

#include "D3DApplication.h"

class Grid;
class Shape;
class LevelPole;

class BlockOut : public D3DApplication
{
public:
	BlockOut(HINSTANCE hInstance);
	virtual ~BlockOut();

	virtual void InitApplication();
	virtual void OnResize();
	virtual void UpdateScene(float deltaTime);
	virtual void DrawScene();

private:
	void BuildEffect();
	void BuildVertexLayout();
	void BuildBlendStates();
	void BuildDepthStencilState();
	void BuildFont();

	void NewGame();
	void GameOver();

	void ReadHighScore();
	void WriteHighScore() const;

	void SetCurrentAndNextShapes();
	void SetNextShapePreview();

	void DrawGameInfo() const;

	void MoveDownCurrentShape();

	virtual void OnKeyPressed(unsigned key);

	Matrix m_View;
	Matrix m_Projection;

	ID3D10Effect*				m_pEffect;
	ID3D10InputLayout*			m_pVertexLayout;
	ID3D10EffectMatrixVariable*	m_pEffectViewProjection;
	ID3D10BlendState*			m_pTransparentBS;
	ID3D10DepthStencilState*	m_pDepthStencilState;
	ID3DX10Font*				m_pFont;

	Grid*		m_pGrid;
	LevelPole*	m_pLevelPole;
	Shape*		m_pCurrentShape;
	Shape*		m_pNextShape;

	unsigned m_PlayedCubesCount;
	unsigned m_Level;
	unsigned m_Score;
	unsigned m_HighScore;

	// all times are in seconds
	float m_NextLevelStartTime;
	float m_CurrentTimeAfterLastFall;
	float m_ShapeFallingTime;

	unsigned m_LastKeyPressed;

	bool m_IsGamePaused;
	bool m_IsGameOver;
};
