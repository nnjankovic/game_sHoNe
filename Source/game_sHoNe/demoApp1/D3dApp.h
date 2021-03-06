#pragma once
#ifndef D3DAPP_H_
#define D3DAPP_H_

#include"Utils.h"
#include"GameTimer.h"

#include <string>

class D3dApp
{
protected:
	static D3dApp* s_instance;
	
	D3dApp(HINSTANCE hInstance);
	D3dApp(const D3dApp& rhs) = delete;
	D3dApp& operator= (const D3dApp& rhs) = delete;
	virtual ~D3dApp();
public:
	static D3dApp* getInstance() { return s_instance; }
	
	HINSTANCE AppInst() const { return m_hInstance; };
	HWND MainWnd() const { return m_window; };
	float AspectRatio() const { return m_screenWidth / m_screenHeight; };

	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void Run();

	virtual bool Initialize();

protected:
	bool InitMainWindow();
	bool InitDirect3D();
	void CreateCommandObjects();
	void CreateSwapChain();
	void CalculateFrameStats();

	virtual void CreateDescriptorHeaps();
	virtual void OnResize();
	virtual void Update(const GameTimer& gt) = 0;
	virtual void Draw(const GameTimer& gt) = 0;
	
	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
	virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y) { }

	void CreateRTViewAndBuffer();
	void CreateDSViewAndBuffer();

	void FlushCommandQueue();

	Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;

	Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_d3dFence;
	int m_currentFenceValue;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_d3dCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_d3dCommandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_d3dCommandList;

	bool      m_4xMsaaState = false;
	unsigned int m_4xMsaaQuality = 0;

	unsigned int m_RtvDescriptorSize; // render target view descriptor size
	unsigned int m_DsvDescriptorSize; // depth/stencil view descriptor size
	unsigned int m_CrvSrvUrvDescriptorSize; // constant buffers, shader resources, unordered access view resources

	DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	static const int s_backBufferCount = 2;
	int m_currentBackBuffer = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_d3dSwapChainBuffers[s_backBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> m_d3dDepthStencilBuffer;

	Microsoft::WRL::ComPtr<IDXGISwapChain> m_d3dSwapChain;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_d3dRTVDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_d3dDSVDescriptorHeap;

	D3D12_VIEWPORT m_viewPort;
	D3D12_RECT m_scissorRect;
	HWND m_window;

private:
	HINSTANCE m_hInstance;
	const std::wstring m_windowClassName = L"D3DAppMainWindow";
	std::wstring m_mainWndCaption = L"d3d App";

	int m_screenWidth;
	int m_screenHeight;

	bool m_isAppPaused = false;
	bool m_isMaximized = false;
	bool m_isMinimized = false;
	bool m_isResizing = false;

	GameTimer m_timer;
};

#endif
