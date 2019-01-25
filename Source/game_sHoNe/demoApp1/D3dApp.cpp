#include "D3dApp.h"
#include <WindowsX.h>

using Microsoft::WRL::ComPtr;
using namespace std;
using namespace DirectX;

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

	hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory));
	if (FAILED(hr))
		return false;

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
	m_4xMsaaQuality = multisampleQuality.NumQualityLevels;
	assert(multisampleQuality.NumQualityLevels > 0);

	CreateCommandObjects();
	CreateSwapChain();
	CreateDescriptorHeaps();
}

void D3dApp::CreateCommandObjects() {
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	
	HRESULT hr = m_d3dDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_d3dCommandQueue));
	assert(!FAILED(hr));

	hr = m_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_d3dCommandAllocator.GetAddressOf()));
	assert(!FAILED(hr));

	hr = m_d3dDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_d3dCommandAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&m_d3dCommandList)
		);
	assert(!FAILED(hr));
	m_d3dCommandList->Close();
}

void D3dApp::CreateSwapChain() {

	m_d3dSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	
	swapChainDesc.BufferDesc.Height = m_screenHeight;
	swapChainDesc.BufferDesc.Width = m_screenWidth;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = m_backBufferFormat;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferCount = D3dApp::s_backBufferCount;
	swapChainDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
	swapChainDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDesc.OutputWindow = m_window;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Windowed = true;

	HRESULT hr = m_dxgiFactory->CreateSwapChain(
		m_d3dCommandQueue.Get(),
		&swapChainDesc,
		m_d3dSwapChain.GetAddressOf()
	);
	assert(!FAILED(hr));

}

void D3dApp::CreateDescriptorHeaps() {
	HRESULT hr;
	
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = s_backBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.NodeMask = 0;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = m_d3dDevice->CreateDescriptorHeap(
		&rtvHeapDesc,
		IID_PPV_ARGS(&m_d3dRTVDescriptorHeap)
	);
	assert(!FAILED(hr));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.NodeMask = 0;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = m_d3dDevice->CreateDescriptorHeap(
		&dsvHeapDesc,
		IID_PPV_ARGS(&m_d3dDSVDescriptorHeap)
	);
	assert(!FAILED(hr));
}

void D3dApp::CreateRTViewAndBuffer() {

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_d3dRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	for (int i = 0; i < s_backBufferCount; i++)
	{
		HRESULT hr = m_d3dSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_d3dSwapChainBuffers[i]));
		assert(!FAILED(hr));
		m_d3dDevice->CreateRenderTargetView(m_d3dSwapChainBuffers[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, m_RtvDescriptorSize);
	}
}

void D3dApp::CreateDSViewAndBuffer() {

	D3D12_RESOURCE_DESC depthStencilDesc;

	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = m_screenWidth;
	depthStencilDesc.Height = m_screenHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = m_depthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	HRESULT hr = m_d3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(m_d3dDepthStencilBuffer.GetAddressOf()));
	assert(!FAILED(hr));
}

void D3dApp::FlushCommandQueue() {

	m_currentFenceValue++;

	HRESULT hr = m_d3dCommandQueue->Signal(m_d3dFence.Get(), m_currentFenceValue);
	assert(!FAILED(hr));

	if (m_d3dFence->GetCompletedValue() < m_currentFenceValue)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		m_d3dFence->SetEventOnCompletion(m_currentFenceValue, eventHandle);
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

void D3dApp::OnResize() {
	assert(m_d3dDevice);
	assert(m_d3dSwapChain);
	assert(m_d3dCommandAllocator);

	FlushCommandQueue();

	HRESULT hr = m_d3dCommandList->Reset(m_d3dCommandAllocator.Get(), nullptr);
	assert(!FAILED(hr));

	for (int i = 0; i < s_backBufferCount; i++)
	{
		m_d3dSwapChainBuffers[i].Reset();
	}
	m_d3dDepthStencilBuffer.Reset();

	hr = m_d3dSwapChain->ResizeBuffers(
		s_backBufferCount,
		m_screenWidth, m_screenHeight,
		m_backBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	);
	assert(!FAILED(hr));

	m_currentBackBuffer = 0;

	CreateRTViewAndBuffer();
	CreateDSViewAndBuffer();

	m_d3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_d3dDepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	hr = m_d3dCommandList->Close();
	assert(!FAILED(hr));

	ID3D12CommandList* cmdsLists[] = { m_d3dCommandList.Get() };
	m_d3dCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	FlushCommandQueue();
}