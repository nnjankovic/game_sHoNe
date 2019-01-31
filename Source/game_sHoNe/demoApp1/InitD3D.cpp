#include "InitD3D.h"
#include <DirectXColors.h>

using namespace DirectX;


InitD3D::InitD3D(HINSTANCE hInstance) : D3dApp(hInstance)
{
}


InitD3D::~InitD3D()
{
}

bool InitD3D::Initialize() {
	return D3dApp::Initialize();
}

void InitD3D::OnResize() {
	return D3dApp::OnResize();
}

void InitD3D::Update(const GameTimer& gt) {

}

void InitD3D::Draw(const GameTimer& gt) {
	
	HRESULT hr = m_d3dCommandAllocator->Reset();
	assert(!(FAILED(hr)));

	hr = m_d3dCommandList->Reset(m_d3dCommandAllocator.Get(), nullptr);
	assert(!(FAILED(hr)));

	m_d3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_d3dSwapChainBuffers[m_currentBackBuffer].Get(),
																				D3D12_RESOURCE_STATE_PRESENT, //state before
																				D3D12_RESOURCE_STATE_RENDER_TARGET));	//state after
	m_d3dCommandList->RSSetViewports(1, &m_viewPort);
	m_d3dCommandList->RSSetScissorRects(1, &m_scissorRect);


	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(m_d3dRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_currentBackBuffer, m_RtvDescriptorSize);
	m_d3dCommandList->ClearRenderTargetView(rtv, Colors::IndianRed, 0, nullptr);
	m_d3dCommandList->ClearDepthStencilView(m_d3dDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	m_d3dCommandList->OMSetRenderTargets(1, &rtv, true, &m_d3dDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	m_d3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_d3dSwapChainBuffers[m_currentBackBuffer].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, //state before
		D3D12_RESOURCE_STATE_PRESENT));	//state after

	m_d3dCommandList->Close();

	ID3D12CommandList* ppCommandLists[] = { m_d3dCommandList.Get() };
	m_d3dCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	m_d3dSwapChain->Present(0, 0);

	m_currentBackBuffer = (m_currentBackBuffer + 1) % s_backBufferCount;

	FlushCommandQueue();
}
