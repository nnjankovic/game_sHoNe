#include "D3dApp.h"

D3dApp* D3dApp::s_instance = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	return 0;
}

D3dApp::D3dApp(HINSTANCE hInstance) : m_hInstance(hInstance),
									  m_timer(),
									  m_screenHeight(600),
									  m_screenWidth(800),
									  m_currentFenceValue(0)
{
	s_instance = this;
}

bool D3dApp::InitMainWindow() {
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = m_windowClassName.c_str();

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	RECT rectangle{ 0, 0, m_screenWidth, m_screenHeight };
	AdjustWindowRect(&rectangle, WS_OVERLAPPEDWINDOW, false);
	int width = rectangle.right - rectangle.left;
	int height = rectangle.bottom - rectangle.top;

	m_window = CreateWindow(m_windowClassName.c_str(), L"d3d MainWindow", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_hInstance, 0);

	if (!m_window)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(m_window, SW_SHOW);
	UpdateWindow(m_window);

	return true;
}

void D3dApp::Run() {
	MSG msg;

	m_timer.Reset();

	while (msg.message != WM_QUIT) 
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_timer.Tick();
			if (!m_isAppPaused)
			{

			}
			else
			{

			}
		}
	}
}

LRESULT  D3dApp::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{
		case WM_ACTIVATE:
		{
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				m_isAppPaused = true;
				m_timer.Pause();
			}
			else
			{
				m_isAppPaused = false;
				m_timer.UnPause();
			}
		}
		break;

		case WM_SIZE:

			break;

		case WM_ENTERSIZEMOVE:
			break;

		case WM_EXITSIZEMOVE:
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_MENUCHAR:
			return MAKELRESULT(0, MNC_CLOSE);

		case WM_GETMINMAXINFO:
			break;

		case WM_LBUTTONDOWN:
			break;
		case WM_RBUTTONDOWN:
			break;
		case WM_MBUTTONDOWN:
			break;

		case WM_LBUTTONUP:
			break;
		case WM_RBUTTONUP:
			break;
		case WM_MBUTTONUP:
			break;

		case WM_MOUSEMOVE:
			break;

		case WM_KEYUP:
			if (wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
			}
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
	}

	return 0;
}

bool D3dApp::InitDirect3D() {

	HRESULT hr;
	hr = D3D12CreateDevice(nullptr,	//display adapter if nullptr use primary 
						   D3D_FEATURE_LEVEL_11_0, //minimum feature level
						   IID_PPV_ARGS(&m_d3dDevice)); // macro that holds COM ID of parameter and parameter
	if (FAILED(hr))
		return false;

	hr = m_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_d3dFence));
	if (FAILED(hr))
		return false;

	m_RtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_DsvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_CrvSrvUrvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//Check multisample quality
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS multisampleQuality;
	multisampleQuality.Format = m_backBufferFormat;
	multisampleQuality.NumQualityLevels = 0;
	multisampleQuality.SampleCount = 4;
	multisampleQuality.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	m_d3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&multisampleQuality,
		sizeof(&multisampleQuality)
	);
	assert(multisampleQuality.NumQualityLevels > 0);



}

void D3dApp::CreateCommandObjects() {
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	
	HRESULT hr = m_d3dDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_d3dCommandQueue));
	assert(!FAILED(hr));



}