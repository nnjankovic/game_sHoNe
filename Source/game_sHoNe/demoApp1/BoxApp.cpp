#include "BoxApp.h"
#include "MathHelper.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

BoxApp::BoxApp(HINSTANCE hInstance)
	: D3dApp(hInstance),
	m_CbvHeap(nullptr),
	m_ObjectConstantBuffer(nullptr),
	m_RootSignature(nullptr),
	m_BoxGeometry(nullptr),
	m_PipelineStateObject(nullptr)
{
}

BoxApp::~BoxApp()
{
}

bool BoxApp::Initialize() {
	if (!D3dApp::Initialize())
		return false;

	HRESULT hr = m_d3dCommandList->Reset(m_d3dCommandAllocator.Get(), nullptr);
	assert(!(FAILED(hr)));

	BuildDescriptorHeaps();
	BuildConstantBuffers();
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildBoxGeometry();
	BuildPyramidGeometry();
	BuildGridGeometry();
	BuildPipelineStateObject();

	m_d3dCommandList->Close();
	ID3D12CommandList* cmdsLists[] = { m_d3dCommandList.Get() };
	m_d3dCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	FlushCommandQueue();

	return true;
}

void BoxApp::BuildDescriptorHeaps() {
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;

	HRESULT hr = m_d3dDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_CbvHeap));
	assert(!(FAILED(hr)));
}

void BoxApp::BuildConstantBuffers() {
	m_ObjectConstantBuffer = std::make_unique<UploadBuffer<ObjectConstants>>(m_d3dDevice.Get(), 1, true);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = m_ObjectConstantBuffer->Resource()->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = m_ObjectConstantBuffer->getElementSize();

	m_d3dDevice->CreateConstantBufferView(
		&cbvDesc,
		m_CbvHeap->GetCPUDescriptorHandleForHeapStart()
	);
}

void BoxApp::BuildRootSignature() {
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

void BoxApp::BuildShadersAndInputLayout() {

	m_VertexShaderByteCode = D3DUtils::CompileShader(L"Shader.hlsl", nullptr, "VS", "vs_5_0");
	m_PixelShaderByteCode =	 D3DUtils::CompileShader(L"Shader.hlsl", nullptr, "PS", "ps_5_0");

	m_InputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}


void BoxApp::BuildGridGeometry() {
	int n = 9;
	int m = 9;

	int numOfVertices = n*m;

	std::vector<Vertex> vertices;

	for(int i=0; i<n; i++)
		for (int j = 0; j < m; j++)
		{
			float z = (n - 1) / 2 - i;
			float y = 0;
			float x = j - (m - 1) / 2;

			vertices.push_back(
				Vertex({ XMFLOAT3(x, y,z), XMFLOAT4(Colors::Aqua) })
			);
		}

	std::vector<std::uint16_t> indices;
	int numOfIndices = (n - 1)*(m - 1) * 2;

	for(std::uint16_t i=0; i<n-1; i++)
		for (std::uint16_t j = 0; j < m - 1; j++)
		{
			indices.push_back(i*n+j);
			indices.push_back(i*n + j + 1);
			indices.push_back((i + 1)*n + j);

			indices.push_back((i + 1)*n + j);
			indices.push_back(i*n + j + 1);
			indices.push_back((i + 1)*n + j + 1);
		}


	const UINT vertexBufferSize = static_cast<UINT>(vertices.size()) * sizeof(Vertex);
	const UINT indexBufferSize = static_cast<UINT>(indices.size()) * sizeof(std::uint16_t);

	m_GridGeometry = std::make_unique<D3DUtils::MeshGeometry>();
	m_GridGeometry->name = "gridGeo";

	//TODO: Check if this is necessary
	HRESULT hr = D3DCreateBlob(vertexBufferSize, &m_GridGeometry->VertexBufferCPU);
	CopyMemory(m_GridGeometry->VertexBufferCPU->GetBufferPointer(), vertices.data(), vertexBufferSize);
	assert(!(FAILED(hr)));

	hr = D3DCreateBlob(indexBufferSize, &m_GridGeometry->IndexBufferCPU);
	CopyMemory(m_GridGeometry->IndexBufferCPU->GetBufferPointer(), indices.data(), indexBufferSize);
	assert(!(FAILED(hr)));

	m_GridGeometry->VertexBufferGPU = D3DUtils::CreateDefaultBuffer(m_d3dDevice.Get(), m_d3dCommandList.Get(),
		vertices.data(), vertexBufferSize, m_GridGeometry->VertexBufferUploader);

	m_GridGeometry->IndexBufferGPU = D3DUtils::CreateDefaultBuffer(m_d3dDevice.Get(),
		m_d3dCommandList.Get(), indices.data(), indexBufferSize, m_GridGeometry->IndexBufferUploader);

	m_GridGeometry->VertexByteStride = sizeof(Vertex);
	m_GridGeometry->VertexBufferByteSize = vertexBufferSize;
	m_GridGeometry->IndexFormat = DXGI_FORMAT_R16_UINT;
	m_GridGeometry->IndexBufferByteSize = indexBufferSize;

	D3DUtils::SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	m_GridGeometry->DrawArgs["grid"] = submesh;
}


void BoxApp::BuildPyramidGeometry() {
	std::array<Vertex, 5> vertices = {
		Vertex({ XMFLOAT3(-1.0f, +2.0f, +1.0f), XMFLOAT4(Colors::Green) }),
		Vertex({ XMFLOAT3(+1.0f, +2.0f, +1.0f), XMFLOAT4(Colors::Green) }),
		Vertex({ XMFLOAT3(-1.0f, +2.0f, -1.0f), XMFLOAT4(Colors::Green) }),
		Vertex({ XMFLOAT3(+1.0f, +2.0f, -1.0f), XMFLOAT4(Colors::Green) }),
		Vertex({ XMFLOAT3(0.0f, +4.0f, 0.0f), XMFLOAT4(Colors::Red) }),
	};

	std::array<std::uint16_t, 18> indices = {
		2, 1, 0,
		1, 2, 3,
		4, 3, 2,
		1, 3, 4,
		0, 1, 4,
		4, 2, 0
	};

	const UINT vertexBufferSize = static_cast<UINT>(vertices.size()) * sizeof(Vertex);
	const UINT indexBufferSize = static_cast<UINT>(indices.size()) * sizeof(std::uint16_t);

	m_PyramidGeometry = std::make_unique<D3DUtils::MeshGeometry>();
	m_PyramidGeometry->name = "pyramidGeo";

	//TODO: Check if this is necessary
	HRESULT hr = D3DCreateBlob(vertexBufferSize, &m_PyramidGeometry->VertexBufferCPU);
	CopyMemory(m_PyramidGeometry->VertexBufferCPU->GetBufferPointer(), vertices.data(), vertexBufferSize);
	assert(!(FAILED(hr)));

	hr = D3DCreateBlob(indexBufferSize, &m_PyramidGeometry->IndexBufferCPU);
	CopyMemory(m_PyramidGeometry->IndexBufferCPU->GetBufferPointer(), indices.data(), indexBufferSize);
	assert(!(FAILED(hr)));

	m_PyramidGeometry->VertexBufferGPU = D3DUtils::CreateDefaultBuffer(m_d3dDevice.Get(), m_d3dCommandList.Get(),
		vertices.data(), vertexBufferSize, m_PyramidGeometry->VertexBufferUploader);

	m_PyramidGeometry->IndexBufferGPU = D3DUtils::CreateDefaultBuffer(m_d3dDevice.Get(),
		m_d3dCommandList.Get(), indices.data(), indexBufferSize, m_PyramidGeometry->IndexBufferUploader);

	m_PyramidGeometry->VertexByteStride = sizeof(Vertex);
	m_PyramidGeometry->VertexBufferByteSize = vertexBufferSize;
	m_PyramidGeometry->IndexFormat = DXGI_FORMAT_R16_UINT;
	m_PyramidGeometry->IndexBufferByteSize = indexBufferSize;

	D3DUtils::SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	m_PyramidGeometry->DrawArgs["pyramid"] = submesh;

}

void BoxApp::BuildBoxGeometry() {
	std::array<Vertex, 8> vertices =
	{
		Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Blue) }),
		Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Turquoise) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Goldenrod) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Salmon) })
	};

	std::array<std::uint16_t, 36> indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	const UINT vertexBufferSize = static_cast<UINT>(vertices.size()) * sizeof(Vertex);
	const UINT indexBufferSize = static_cast<UINT>(indices.size()) * sizeof(std::uint16_t);

	m_BoxGeometry = std::make_unique<D3DUtils::MeshGeometry>();
	m_BoxGeometry->name = "boxGeo";

	//TODO: Check if this is necessary
	/*HRESULT hr = D3DCreateBlob(vertexBufferSize, &m_BoxGeometry->VertexBufferCPU);
	CopyMemory(m_BoxGeometry->VertexBufferCPU->GetBufferPointer(), vertices.data(), vertexBufferSize);
	assert(!(FAILED(hr)));

	hr = D3DCreateBlob(indexBufferSize, &m_BoxGeometry->IndexBufferCPU);
	CopyMemory(m_BoxGeometry->IndexBufferCPU->GetBufferPointer(), indices.data(), indexBufferSize);
	assert(!(FAILED(hr)));*/

	m_BoxGeometry->VertexBufferGPU = D3DUtils::CreateDefaultBuffer(m_d3dDevice.Get(), m_d3dCommandList.Get(),
		vertices.data(), vertexBufferSize, m_BoxGeometry->VertexBufferUploader);

	m_BoxGeometry->IndexBufferGPU = D3DUtils::CreateDefaultBuffer(m_d3dDevice.Get(),
		m_d3dCommandList.Get(), indices.data(), indexBufferSize, m_BoxGeometry->IndexBufferUploader);

	m_BoxGeometry->VertexByteStride = sizeof(Vertex);
	m_BoxGeometry->VertexBufferByteSize = vertexBufferSize;
	m_BoxGeometry->IndexFormat = DXGI_FORMAT_R16_UINT;
	m_BoxGeometry->IndexBufferByteSize = indexBufferSize;

	D3DUtils::SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	m_BoxGeometry->DrawArgs["box"] = submesh;
}

void BoxApp::BuildPipelineStateObject() {
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { m_InputLayout.data(), (UINT)m_InputLayout.size() };
	psoDesc.pRootSignature = m_RootSignature.Get();
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_VertexShaderByteCode->GetBufferPointer()),
		m_VertexShaderByteCode->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_PixelShaderByteCode->GetBufferPointer()),
		m_PixelShaderByteCode->GetBufferSize()
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
	m_d3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineStateObject));
}

void BoxApp::OnResize() {
	D3dApp::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*XM_PI, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void BoxApp::Update(const GameTimer& gt) {
    float x = m_radius*cosf(m_alpha)*cosf(m_beta);
    float z = m_radius*cosf(m_alpha)*sinf(m_beta);
    float y = m_radius*sinf(m_alpha);

    // Build the view matrix.
    XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);

	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world*view*proj;

	ObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
	m_ObjectConstantBuffer->CopyData(0, objConstants);
}

void BoxApp::Draw(const GameTimer& gt){

	HRESULT hr = m_d3dCommandAllocator->Reset();
	assert(!(FAILED(hr)));

	hr = m_d3dCommandList->Reset(m_d3dCommandAllocator.Get(), nullptr/*m_PipelineStateObject.Get()*/);
	m_d3dCommandList->RSSetViewports(1, &m_viewPort);
	m_d3dCommandList->RSSetScissorRects(1, &m_scissorRect);

	m_d3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_d3dSwapChainBuffers[m_currentBackBuffer].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(m_d3dRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_currentBackBuffer, m_RtvDescriptorSize);
	m_d3dCommandList->ClearRenderTargetView(rtv, Colors::White, 0, nullptr);
	m_d3dCommandList->ClearDepthStencilView(m_d3dDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	m_d3dCommandList->OMSetRenderTargets(1, &rtv, true, &m_d3dDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_CbvHeap.Get() };
	m_d3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_d3dCommandList->SetGraphicsRootSignature(m_RootSignature.Get());

	m_d3dCommandList->SetPipelineState(m_PipelineStateObject.Get());

	m_d3dCommandList->IASetVertexBuffers(0, 1, &m_BoxGeometry->VertexBufferView());
	m_d3dCommandList->IASetIndexBuffer(&m_BoxGeometry->IndexBufferView());
	m_d3dCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_d3dCommandList->SetGraphicsRootDescriptorTable(0, m_CbvHeap->GetGPUDescriptorHandleForHeapStart());
	m_d3dCommandList->DrawIndexedInstanced(m_BoxGeometry->DrawArgs["box"].IndexCount, 1, 0, 0, 0);

	/*m_d3dCommandList->IASetVertexBuffers(0, 1, &m_PyramidGeometry->VertexBufferView());
	m_d3dCommandList->IASetIndexBuffer(&m_PyramidGeometry->IndexBufferView());
	m_d3dCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_d3dCommandList->SetGraphicsRootDescriptorTable(0, m_CbvHeap->GetGPUDescriptorHandleForHeapStart());
	m_d3dCommandList->DrawIndexedInstanced(m_PyramidGeometry->DrawArgs["pyramid"].IndexCount, 1, 0, 0, 0);*/


	m_d3dCommandList->IASetVertexBuffers(0, 1, &m_GridGeometry->VertexBufferView());
	m_d3dCommandList->IASetIndexBuffer(&m_GridGeometry->IndexBufferView());
	m_d3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_d3dCommandList->SetGraphicsRootDescriptorTable(0, m_CbvHeap->GetGPUDescriptorHandleForHeapStart());
	m_d3dCommandList->DrawIndexedInstanced(m_GridGeometry->DrawArgs["grid"].IndexCount, 1, 0, 0, 0);

	m_d3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_d3dSwapChainBuffers[m_currentBackBuffer].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, //state before
		D3D12_RESOURCE_STATE_PRESENT));	//state after

	hr = m_d3dCommandList->Close();
	assert(!(FAILED(hr)));

	ID3D12CommandList* cmdsLists[] = { m_d3dCommandList.Get() };
	m_d3dCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	hr = m_d3dSwapChain->Present(0, 0);
	m_currentBackBuffer = (m_currentBackBuffer + 1) % s_backBufferCount;

	FlushCommandQueue();
}

void BoxApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	SetCapture(m_window);
}

void BoxApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void BoxApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - m_LastMousePos.y));

		// Update angles based on input to orbit camera around box.
		m_alpha += dy;
		m_beta += dx;

		// Restrict the angle mPhi.
		//mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
		//mTheta = MathHelper::Clamp(mTheta, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.005f*static_cast<float>(x - m_LastMousePos.x);
		float dy = 0.005f*static_cast<float>(y - m_LastMousePos.y);

		// Update the camera radius based on input.
		m_radius += dx - dy;

		// Restrict the radius.
		//m_radius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}