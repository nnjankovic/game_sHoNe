#pragma once
#ifndef D3D12_RENDERER_H
#define D3D12_RENDERER_H

#include "IRenderer.h"
#include "Utility.h"

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace DirectX::PackedVector;

struct ObjectConstants
{
	XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
};

class D3D12Renderer : public IRenderer {
public:
	D3D12Renderer(HINSTANCE hInstance);

	bool Init() override;
	bool Exit() override;

	bool PrepareDraw() override;
	bool Draw(DrawItem& drawItem) override;
	bool UploadStaticGeometry(std::vector<std::shared_ptr<DrawItem>> staticDrawItems) override;
	bool UploadTexture(Texture& texture) override;
	void Present() override;
	void createPSO(DrawItem& drawItem) override;

	void setWindowTitle(std::wstring title) override;

	LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	std::vector<CD3DX12_STATIC_SAMPLER_DESC> GetStaticSamplers();

	~D3D12Renderer() {};

private:
	bool InitMainWindow();
	bool InitD3D();
	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateDescriptorHeaps();
	void CreateRTViewAndBuffer();
	void CreateDSViewAndBuffer();

	void CreateConstantBuffers();
	void CreateRootSignature();

	void FlushCommandQueue();
	void OnResize();
private:
	ComPtr<IDXGIFactory4> m_dxgiFactory;
	ComPtr<ID3D12Device>  m_d3dDevice;

	ComPtr<ID3D12Fence>	  m_d3dFence;
	int	m_currentFenceValue = 0;

	ComPtr<ID3D12CommandQueue>			m_d3dCommandQueue;
	ComPtr<ID3D12CommandAllocator>		m_d3dCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList>	m_d3dCommandList;

	ComPtr<IDXGISwapChain>	m_d3dSwapChain;

	unsigned int m_RtvDescriptorSize; // render target view descriptor size
	unsigned int m_DsvDescriptorSize; // depth/stencil view descriptor size
	unsigned int m_CrvSrvUrvDescriptorSize; // constant buffers, shader resources, unordered access view resources

	bool      m_4xMsaaState = false;
	unsigned int m_4xMsaaQuality = 0;

	//BackBuffer info
	DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const int	s_BackBufferCount = 2;
	int			m_currentBackBuffer = 0;
	ComPtr<ID3D12Resource> m_d3dSwapChainBuffers[s_BackBufferCount];

	ComPtr<ID3D12DescriptorHeap> m_d3dRTVDescriptorHeap;
	ComPtr<ID3D12DescriptorHeap> m_d3dDSVDescriptorHeap;
	ComPtr<ID3D12DescriptorHeap> m_CbvHeap;
	ComPtr<ID3D12DescriptorHeap> m_SrvDescriptorHeap;

	ComPtr<ID3D12RootSignature> m_RootSignature;

	ComPtr<ID3D12Resource> m_d3dDepthStencilBuffer;
	DXGI_FORMAT m_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	HWND m_window;
	HINSTANCE m_hInstance;
	const std::wstring m_windowClassName = L"D3D12 Renderer window";
	std::wstring m_mainWndCaption = L"NENAD JANKOVIC";

	int m_screenWidth;
	int m_screenHeight;
	D3D12_VIEWPORT m_viewPort;
	D3D12_RECT m_scissorRect;

	int m_textureCount;

	bool m_isResizing = false;

	std::unique_ptr<uplooad_helper<ObjectConstants>> m_ObjectConstantBuffer;

	XMFLOAT4X4 m_projectionMatrix = MathHelper::Identity4x4();
	
	//Depends on height/width ratio (screen size) used to project 3d vertices to 2d screen space
};

#endif

