#include "pch.h"
#include "D3DApplication.h"

const char* APPLICATION_NAME = "Block Out";
const char* MAIN_WINDOW_CLASS_NAME = "D3DWindowClass";

const int MAIN_WINDOW_WIDTH = 800;
const int MAIN_WINDOW_HEIGHT = 600;

LRESULT CALLBACK MainWindowProcedure(HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam)
{
	static D3DApplication* pApplication = 0;

	switch (message)
	{
		case WM_CREATE:
		{
			// Get the 'this' pointer we passed to CreateWindow via the lpParam parameter.
			CREATESTRUCT* pCreateStruct = (CREATESTRUCT*)lParam;
			pApplication = static_cast<D3DApplication*>(pCreateStruct->lpCreateParams);
			return 0;
		}
	}

	// Don't start processing messages until after WM_CREATE.
	if (pApplication)
	{
		return pApplication->MessageProcedure(message, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hWindow, message, wParam, lParam);
	}
}

D3DApplication::D3DApplication(HINSTANCE hInstance)
	: m_hApplicationInstance(hInstance)
	, m_hMainWindow(nullptr)
	, m_IsPaused(false)
	, m_IsMinimized(false)
	, m_IsMaximized(false)
	, m_IsResizing(false)
	, m_pDevice(nullptr)
	, m_pSwapChain(nullptr)
	, m_pDepthStencilBuffer(nullptr)
	, m_pRenderTargetView(nullptr)
	, m_pDepthStencilView(nullptr)
	, m_DriverType(D3D10_DRIVER_TYPE_HARDWARE)
	, m_MainWindowCaption(APPLICATION_NAME)
	, m_ClearColor(BLACK)
	, m_ClientWidth(MAIN_WINDOW_WIDTH)
	, m_ClientHeight(MAIN_WINDOW_HEIGHT)
	, m_FramesPerSecond(0)
	, m_MillisecondsPerFrame(0.0)
{
}

D3DApplication::~D3DApplication()
{
	SafeRelease(m_pRenderTargetView);
	SafeRelease(m_pDepthStencilView);
	SafeRelease(m_pSwapChain);
	SafeRelease(m_pDepthStencilBuffer);
	SafeRelease(m_pDevice);
}

HINSTANCE D3DApplication::GetApplicationInstance() const
{
	return m_hApplicationInstance;
}

HWND D3DApplication::GetMainWindow() const
{
	return m_hMainWindow;
}

int D3DApplication::Run()
{
	MSG message = {0};

	m_GameTimer.Reset();

	while (message.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		// Otherwise, do animation/game stuff.
		else
		{
			m_GameTimer.Tick();

			if(!m_IsPaused)
			{
				UpdateScene(m_GameTimer.GetDeltaTime());	
			}
			else
			{
				Sleep(50);
			}

			DrawScene();
		}
	}

	return int(message.wParam);
}

void D3DApplication::InitApplication()
{
	InitMainWindow();
	InitDirect3D();
}

void D3DApplication::OnResize()
{
	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.

	SafeRelease(m_pRenderTargetView);
	SafeRelease(m_pDepthStencilView);
	SafeRelease(m_pDepthStencilBuffer);

	// Resize the swap chain and recreate the render target view.
	HR(m_pSwapChain->ResizeBuffers(1, m_ClientWidth, m_ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

	ID3D10Texture2D* backBuffer;
	HR(m_pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(m_pDevice->CreateRenderTargetView(backBuffer, 0, &m_pRenderTargetView));
	SafeRelease(backBuffer);

	// Create the depth/stencil buffer and view.
	D3D10_TEXTURE2D_DESC depthStencilDescription;
	
	depthStencilDescription.Width				= m_ClientWidth;
	depthStencilDescription.Height				= m_ClientHeight;
	depthStencilDescription.MipLevels			= 1;
	depthStencilDescription.ArraySize			= 1;
	depthStencilDescription.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDescription.SampleDesc.Count	= 1; // multisampling must match
	depthStencilDescription.SampleDesc.Quality	= 0; // swap chain values.
	depthStencilDescription.Usage				= D3D10_USAGE_DEFAULT;
	depthStencilDescription.BindFlags			= D3D10_BIND_DEPTH_STENCIL;
	depthStencilDescription.CPUAccessFlags		= 0; 
	depthStencilDescription.MiscFlags			= 0;

	HR(m_pDevice->CreateTexture2D(&depthStencilDescription, 0, &m_pDepthStencilBuffer));
	HR(m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, 0, &m_pDepthStencilView));

	// Bind the render target view and depth/stencil view to the pipeline.
	m_pDevice->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	
	// Set the viewport transform.
	D3D10_VIEWPORT viewPort;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width    = m_ClientWidth;
	viewPort.Height   = m_ClientHeight;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	m_pDevice->RSSetViewports(1, &viewPort);
}

void D3DApplication::UpdateScene( float deltaTime )
{
	ComputeFrameStatistics();

	std::ostringstream framesStatistics;
	framesStatistics.precision(3);
	framesStatistics << " FPS: " << m_FramesPerSecond << " MsPF: " << m_MillisecondsPerFrame;
	m_MainWindowCaption = APPLICATION_NAME + framesStatistics.str();
	SetWindowText(GetMainWindow(), m_MainWindowCaption.c_str());
}

void D3DApplication::DrawScene()
{
	m_pDevice->ClearRenderTargetView(m_pRenderTargetView, m_ClearColor);
	m_pDevice->ClearDepthStencilView(m_pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
}

LRESULT D3DApplication::MessageProcedure(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	// WM_ACTIVATE is sent when the window is activated or deactivated.  
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_IsPaused = true;
			m_GameTimer.Stop();
		}
		else
		{
			m_IsPaused = false;
			m_GameTimer.Start();
		}
		return 0;

	// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		m_ClientWidth  = LOWORD(lParam);
		m_ClientHeight = HIWORD(lParam);
		if (m_pDevice)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				m_IsPaused		= true;
				m_IsMinimized	= true;
				m_IsMaximized	= false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				m_IsPaused		= false;
				m_IsMinimized	= false;
				m_IsMaximized	= true;

				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{
				// Restoring from minimized state?
				if (m_IsMinimized)
				{
					m_IsPaused		= false;
					m_IsMinimized	= false;

					OnResize();
				}
				// Restoring from maximized state?
				else if (m_IsMaximized)
				{
					m_IsPaused		= false;
					m_IsMaximized	= false;

					OnResize();
				}
				else if (m_IsResizing)
				{
					// do nothing
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}

		return 0;

	// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		m_IsPaused		= true;
		m_IsResizing	= true;
		m_GameTimer.Stop();
		return 0;

	// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
	// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		m_IsPaused		= false;
		m_IsResizing	= false;
		m_GameTimer.Start();
		OnResize();
		return 0;
 
	// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	// The WM_MENUCHAR message is sent when a menu is active and the user presses 
	// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

	// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200; 
		return 0;

	case WM_KEYDOWN:
		OnKeyPressed(wParam);
		return 0;
	}

	return DefWindowProc(m_hMainWindow, message, wParam, lParam);
}

void D3DApplication::InitMainWindow()
{
	WNDCLASS windowClass;
	windowClass.style         = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc   = MainWindowProcedure; 
	windowClass.cbClsExtra    = 0;
	windowClass.cbWndExtra    = 0;
	windowClass.hInstance     = m_hApplicationInstance;
	windowClass.hIcon         = LoadIcon(0, IDI_APPLICATION);
	windowClass.hCursor       = LoadCursor(0, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	windowClass.lpszMenuName  = 0;
	windowClass.lpszClassName = MAIN_WINDOW_CLASS_NAME;

	if (!RegisterClass(&windowClass))
	{
		MessageBox(0, "RegisterClass FAILED", 0, 0);
		PostQuitMessage(0);
	}

	DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT rect = {0, 0, m_ClientWidth, m_ClientHeight};
	AdjustWindowRect(&rect, windowStyle, false);
	int width  = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	m_hMainWindow = CreateWindow(
		MAIN_WINDOW_CLASS_NAME,
		m_MainWindowCaption.c_str(),
		windowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		0,
		0,
		m_hApplicationInstance, 
		this); 

	if (!m_hMainWindow)
	{
		MessageBox(0, "CreateWindow FAILED", 0, 0);
		PostQuitMessage(0);
	}

	ShowWindow(m_hMainWindow, SW_SHOW);
	UpdateWindow(m_hMainWindow);
}

void D3DApplication::InitDirect3D()
{
	// Create the device.

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

	HR(D3D10CreateDevice(0, m_DriverType, 0, createDeviceFlags, D3D10_SDK_VERSION, &m_pDevice));

	IDXGIFactory* pFactory;
	HR(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory)));

		// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.
	DXGI_SWAP_CHAIN_DESC swapChainDescription;

	swapChainDescription.BufferDesc.Width					= m_ClientWidth;
	swapChainDescription.BufferDesc.Height					= m_ClientHeight;
	swapChainDescription.BufferDesc.RefreshRate.Numerator	= 60;
	swapChainDescription.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDescription.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDescription.BufferDesc.ScanlineOrdering		= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDescription.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;

	// No multisampling.
	swapChainDescription.SampleDesc.Count					= 1;
	swapChainDescription.SampleDesc.Quality					= 0;

	swapChainDescription.BufferUsage						= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescription.BufferCount						= 1;
	swapChainDescription.OutputWindow						= m_hMainWindow;
	swapChainDescription.Windowed							= true;
	swapChainDescription.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
	swapChainDescription.Flags								= 0;
	
	HR(pFactory->CreateSwapChain(m_pDevice, &swapChainDescription, &m_pSwapChain));
	HR(pFactory->MakeWindowAssociation(m_hMainWindow, DXGI_MWA_NO_WINDOW_CHANGES));

	// The remaining steps that need to be carried out for d3d creation
	// also need to be executed every time the window is resized. So
	// just call the OnResize method here to avoid code duplication.

	OnResize();
}

void D3DApplication::ComputeFrameStatistics()
{
	static int framesCount = 0;
	static double baseTime = 0.0;

	++framesCount;

	// Compute averages over one second period.
	if ((m_GameTimer.GetGameTime() - baseTime) >= 1.0)
	{
		m_FramesPerSecond = framesCount;
		m_MillisecondsPerFrame = 1000.0 / framesCount;
		
		// Reset for next average.
		framesCount = 0;
		baseTime += 1.0f;
	}
}
