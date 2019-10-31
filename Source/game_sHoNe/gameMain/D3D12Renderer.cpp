#include "D3D12Renderer.h"
#include <WindowsX.h>
#include "DrawItem.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	D3D12Renderer* d3d12Renderer = NULL;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *cs = (CREATESTRUCT*)lParam;
		d3d12Renderer = (D3D12Renderer*)cs->lpCreateParams;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)d3d12Renderer);
	}
	else {
		d3d12Renderer = (D3D12Renderer*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (d3d12Renderer)
			return d3d12Renderer->windowProc(hWnd, message, wParam, lParam);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

D3D12Renderer::D3D12Renderer(HINSTANCE hInstance) :
	m_hInstance(hInstance),
	m_screenHeight(600),
	m_screenWidth(800)
{

}

bool D3D12Renderer::InitMainWindow() {
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

	m_window = CreateWindow(m_windowClassName.c_str(), L"d3d MainWindow", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_hInstance, this);

	if (!m_window)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(m_window, SW_SHOW);
	UpdateWindow(m_window);

	return true;

}

bool D3D12Renderer::Init() {
	if (!InitMainWindow())
		return false;

	if (!InitD3D())
		return false;

	OnResize();

	return true;
}

bool D3D12Renderer::Exit()
{
	return false;
}

bool D3D12Renderer::PrepareDraw()
{
	HRESULT hr = m_d3dCommandAllocator->Reset();
	assert(!(FAILED(hr)));

	hr = m_d3dCommandList->Reset(m_d3dCommandAllocator.Get(), nullptr);
	assert(!(FAILED(hr)));

	m_d3dCommandList->RSSetViewports(1, &m_viewPort);
	m_d3dCommandList->RSSetScissorRects(1, &m_scissorRect);

	m_d3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_d3dSwapChainBuffers[m_currentBackBuffer].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET));


	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(m_d3dRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_currentBackBuffer, m_RtvDescriptorSize);
	m_d3dCommandList->ClearRenderTargetView(rtv, DirectX::Colors::Aqua, 0, nullptr);
	m_d3dCommandList->ClearDepthStencilView(m_d3dDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	m_d3dCommandList->OMSetRenderTargets(1, &rtv, true, &m_d3dDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());


	ID3D12DescriptorHeap* descriptorHeaps[] = { m_CbvHeap.Get() };
	m_d3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_d3dCommandList->SetGraphicsRootSignature(m_RootSignature.Get());


	return true;
}

bool D3D12Renderer::Draw(const DrawItem & drawItem)
{
	XMMATRIX transformMatrix = XMLoadFloat4x4(&drawItem.m_properties.objectConstants.WorldViewProj);
	XMMATRIX proj = XMLoadFloat4x4(&m_projectionMatrix);

	transformMatrix *= proj;

	ObjectConstants bla;

	XMStoreFloat4x4(&bla.WorldViewProj, XMMatrixTranspose(transformMatrix));

	m_d3dCommandList->SetPipelineState(drawItem.m_properties.pipelineStateObject.Get());
	m_ObjectConstantBuffer->setData(bla);

	m_d3dCommandList->IASetVertexBuffers(0, 1, &drawItem.m_geometry.VertexBufferView());
	m_d3dCommandList->IASetIndexBuffer(&drawItem.m_geometry.IndexBufferView());
	m_d3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_d3dCommandList->SetGraphicsRootDescriptorTable(0, m_CbvHeap->GetGPUDescriptorHandleForHeapStart());
	m_d3dCommandList->DrawIndexedInstanced(drawItem.m_geometry.indices.size(), 1, 0, 0, 0);

	return true;
}

bool D3D12Renderer::UploadStaticGeometry(DrawItem & drawItem)
{
	drawItem.m_geometry.VertexBufferGPU = CreateDefaultBuffer(m_d3dDevice.Get(), m_d3dCommandList.Get(),
		drawItem.m_geometry.vertices.data(), drawItem.m_geometry.VertexBufferSize, drawItem.m_geometry.VertexBufferUploader);

	drawItem.m_geometry.IndexBufferGPU = CreateDefaultBuffer(m_d3dDevice.Get(),
		m_d3dCommandList.Get(), drawItem.m_geometry.indices.data(), drawItem.m_geometry.IndexBufferSize, drawItem.m_geometry.IndexBufferUploader);

	m_d3dCommandList->Close();
	ID3D12CommandList* cmdsLists[] = { m_d3dCommandList.Get() };
	m_d3dCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	FlushCommandQueue();

	return true;
}

void D3D12Renderer::Present()
{
	m_d3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_d3dSwapChainBuffers[m_currentBackBuffer].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, //state before
		D3D12_RESOURCE_STATE_PRESENT));	//state after

	m_d3dCommandList->Close();

	ID3D12CommandList* ppCommandLists[] = { m_d3dCommandList.Get() };
	m_d3dCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	m_d3dSwapChain->Present(0, 0);

	m_currentBackBuffer = (m_currentBackBuffer + 1) % s_BackBufferCount;

	FlushCommandQueue();

}

void D3D12Renderer::createPSO(DrawItem & drawItem)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { drawItem.m_properties.inputLayout.data(), (UINT)drawItem.m_properties.inputLayout.size() };
	psoDesc.pRootSignature = m_RootSignature.Get();
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(drawItem.m_properties.vertexShaderByteCode->GetBufferPointer()),
		drawItem.m_properties.vertexShaderByteCode->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(drawItem.m_properties.pixelShaderByteCode->GetBufferPointer()),
		drawItem.m_properties.pixelShaderByteCode->GetBufferSize()
	};
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = m_backBufferFormat;
	psoDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
	psoDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
	psoDesc.DSVFormat = m_depthStencilFormat;
	m_d3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&drawItem.m_properties.pipelineStateObject));
}

bool D3D12Renderer::InitD3D() {

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
		sizeof(multisampleQuality)
	);
	m_4xMsaaQuality = multisampleQuality.NumQualityLevels;
	assert(m_4xMsaaQuality > 0);

	CreateCommandObjects();
	CreateSwapChain();
	CreateDescriptorHeaps();
	CreateConstantBuffers();
	CreateRootSignature();

	return true;
}

void D3D12Renderer::CreateCommandObjects() {
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	HRESULT hr = m_d3dDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_d3dCommandQueue));
	assert(!(FAILED(hr)));

	hr = m_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_d3dCommandAllocator.GetAddressOf()));
	assert(!(FAILED(hr)));

	hr = m_d3dDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_d3dCommandAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&m_d3dCommandList)
	);
	assert(!(FAILED(hr)));
	m_d3dCommandList->Close();
}

void D3D12Renderer::CreateSwapChain() {

	m_d3dSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	swapChainDesc.BufferDesc.Height = m_screenHeight;
	swapChainDesc.BufferDesc.Width = m_screenWidth;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = m_backBufferFormat;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferCount = s_BackBufferCount;
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
	assert(!(FAILED(hr)));
}

void D3D12Renderer::CreateDescriptorHeaps() {
	HRESULT hr;

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = s_BackBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.NodeMask = 0;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = m_d3dDevice->CreateDescriptorHeap(
		&rtvHeapDesc,
		IID_PPV_ARGS(&m_d3dRTVDescriptorHeap)
	);
	assert(!(FAILED(hr)));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.NodeMask = 0;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = m_d3dDevice->CreateDescriptorHeap(
		&dsvHeapDesc,
		IID_PPV_ARGS(&m_d3dDSVDescriptorHeap)
	);
	assert(!(FAILED(hr)));

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;

	m_d3dDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_CbvHeap));
	assert(!(FAILED(hr)));
}

void D3D12Renderer::CreateRTViewAndBuffer() {

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_d3dRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	for (int i = 0; i < s_BackBufferCount; i++)
	{
		HRESULT hr = m_d3dSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_d3dSwapChainBuffers[i]));
		assert(!(FAILED(hr)));
		m_d3dDevice->CreateRenderTargetView(m_d3dSwapChainBuffers[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, m_RtvDescriptorSize);
	}
}

void D3D12Renderer::CreateDSViewAndBuffer() {

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
	assert(!(FAILED(hr)));
}

void D3D12Renderer::CreateConstantBuffers()
{
	m_ObjectConstantBuffer = std::make_unique<uplooad_helper<ObjectConstants>>(m_d3dDevice.Get(), 1, true);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = m_ObjectConstantBuffer->Resource()->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = m_ObjectConstantBuffer->getElementSize();

	m_d3dDevice->CreateConstantBufferView(
		&cbvDesc,
		m_CbvHeap->GetCPUDescriptorHandleForHeapStart()
	);
}

void D3D12Renderer::CreateRootSignature()
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[1];

	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
		1,
		slotRootParameter,
		0,
		nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	);

	ComPtr<ID3DBlob> serializedRootSignature = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSignature.GetAddressOf(),
		errorBlob.GetAddressOf());

	if (nullptr != errorBlob)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	assert(!(FAILED(hr)));

	m_d3dDevice->CreateRootSignature(0,
		serializedRootSignature->GetBufferPointer(),
		serializedRootSignature->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature));
}

void D3D12Renderer::FlushCommandQueue() {
	m_currentFenceValue++;

	HRESULT hr = m_d3dCommandQueue->Signal(m_d3dFence.Get(), m_currentFenceValue);
	assert(!(FAILED(hr)));

	if (m_d3dFence->GetCompletedValue() < m_currentFenceValue)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		m_d3dFence->SetEventOnCompletion(m_currentFenceValue, eventHandle);
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

void D3D12Renderer::OnResize() {
	assert(m_d3dDevice);
	assert(m_d3dSwapChain);
	assert(m_d3dCommandAllocator);

	FlushCommandQueue();

	HRESULT hr = m_d3dCommandList->Reset(m_d3dCommandAllocator.Get(), nullptr);
	assert(!(FAILED(hr)));

	for (int i = 0; i < s_BackBufferCount; i++)
	{
		m_d3dSwapChainBuffers[i].Reset();
	}
	m_d3dDepthStencilBuffer.Reset();

	hr = m_d3dSwapChain->ResizeBuffers(
		s_BackBufferCount,
		m_screenWidth, m_screenHeight,
		m_backBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	);
	assert(!(FAILED(hr)));

	m_currentBackBuffer = 0;

	CreateRTViewAndBuffer();
	CreateDSViewAndBuffer();

	m_d3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_d3dDepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	hr = m_d3dCommandList->Close();
	assert(!(FAILED(hr)));

	ID3D12CommandList* cmdsLists[] = { m_d3dCommandList.Get() };
	m_d3dCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	FlushCommandQueue();

	m_viewPort.TopLeftX = 0;
	m_viewPort.TopLeftY = 0;
	m_viewPort.Width = static_cast<float>(m_screenWidth);
	m_viewPort.Height = static_cast<float>(m_screenHeight);
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;

	m_scissorRect = { 0, 0, m_screenWidth, m_screenHeight };

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*XM_PI, m_screenWidth / m_screenHeight, 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_projectionMatrix, P);
}

LRESULT  D3D12Renderer::windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{
	case WM_SIZE:

		m_screenWidth = LOWORD(lParam);
		m_screenHeight = HIWORD(lParam);
		if (m_d3dDevice)
		{
			if (wParam == SIZE_MAXIMIZED)
			{
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (!m_isResizing)
					OnResize();
			}
		}

		break;

	case WM_ENTERSIZEMOVE:
		m_isResizing = true;
		break;

	case WM_EXITSIZEMOVE:
		m_isResizing = false;
		//OnResize();
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);

	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
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