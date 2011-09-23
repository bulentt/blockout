#pragma once

#include "GameTimer.h"

class D3DApplication
{
public:
	D3DApplication(HINSTANCE hInstance);
	virtual ~D3DApplication();

	HINSTANCE	GetApplicationInstance() const;
	HWND		GetMainWindow() const;

	int Run();

	virtual void InitApplication();
	virtual void OnResize();// reset projection/etc
	virtual void UpdateScene(float deltaTime);
	virtual void DrawScene(); 
	virtual LRESULT MessageProcedure(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	virtual void OnKeyPressed(unsigned key) {}

	void InitMainWindow();
	void InitDirect3D();

	// Computes the average frames per second, and also the 
	// average time it takes to render one frame.
	void ComputeFrameStatistics();

	unsigned	m_FramesPerSecond;
	double		m_MillisecondsPerFrame;

	HINSTANCE	m_hApplicationInstance;
	HWND		m_hMainWindow;
	bool		m_IsPaused;
	bool		m_IsMinimized;
	bool		m_IsMaximized;
	bool		m_IsResizing;

	GameTimer m_GameTimer;

	ID3D10Device*			m_pDevice;
	IDXGISwapChain*			m_pSwapChain;
	ID3D10Texture2D*		m_pDepthStencilBuffer;
	ID3D10RenderTargetView* m_pRenderTargetView;
	ID3D10DepthStencilView* m_pDepthStencilView;

	// Hardware device or reference device? D3DApplication defaults to
	// D3D10_DRIVER_TYPE_HARDWARE.
	D3D10_DRIVER_TYPE m_DriverType;

	// Window title/caption.
	std::string m_MainWindowCaption;

	// Color to clear the background.
	Color m_ClearColor;

	int m_ClientWidth;
	 int m_ClientHeight;
};
