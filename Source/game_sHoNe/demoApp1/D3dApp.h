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
	virtual ~D3dApp() {};

public:
	static D3dApp* getInstance() { return s_instance; }
	
	HINSTANCE AppInst() const { return m_hInstance; };
	HWND MainWnd() const { return m_window; };
	float AspectRatio() const { return m_screenWidth / m_screenHeight; };

	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void Run();

protected:
	bool InitMainWindow();
	bool InitDirect3D();
	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateDescriptorHeaps();
	
	void CreateRTV();

	Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;

	Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_d3dFence;
	int m_currentFenceValue;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_d3dCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_d3dCommandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_d3dCommandList;

	bool      m_4xMsaaState = true;
	unsigned int m_4xMsaaQuality = 0;

	unsigned int m_RtvDescriptorSize; // render target view descriptor size
	unsigned int m_DsvDescriptorSize; // depth/stencil view descriptor size
	unsigned int m_CrvSrvUrvDescriptorSize; // constant buffers, shader resources, unordered access view resources

	DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	static const int s_backBufferCount = 2;
	int m_currentBackBuffer = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_d3dSwapChainBuffers[s_backBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> m_d3dDepthStencilBuffer;

	Microsoft::WRL::ComPtr<IDXGISwapChain> m_d3dSwapChain;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_d3dRTVDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_d3dDSVDescriptorHeap;

private:
	HWND m_window;
	HINSTANCE m_hInstance;
	const std::wstring m_windowClassName = L"D3DAppMainWindow";

	int m_screenWidth;
	int m_screenHeight;
	bool m_isAppPaused;

	GameTimer m_timer;
};

#endif
