#pragma once
#include "D3dApp.h"
#include "UploadBuffer.h"
#include "MathHelper.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

struct ObjectConstants
{
	XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
};

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

class BoxApp :
	public D3dApp
{
public:
	BoxApp(HINSTANCE hInstance);
	~BoxApp();

	void Update(const GameTimer& gt) override;
	void Draw(const GameTimer& gt) override;
	bool Initialize() override;
	void OnMouseDown(WPARAM btnState, int x, int y) override;
	void OnMouseUp(WPARAM btnState, int x, int y) override;
	void OnMouseMove(WPARAM btnState, int x, int y) override;
	void OnResize() override;

private:
	void BuildDescriptorHeaps();
	void BuildConstantBuffers();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();

	void BuildGridGeometry();


	void BuildBoxGeometry();
	void BuildPyramidGeometry();

	void BuildPipelineStateObject();
	
private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CbvHeap;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
	Microsoft::WRL::ComPtr<ID3DBlob> m_VertexShaderByteCode;
	Microsoft::WRL::ComPtr<ID3DBlob> m_PixelShaderByteCode;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineStateObject;

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

	std::unique_ptr<UploadBuffer<ObjectConstants>> m_ObjectConstantBuffer;
	std::unique_ptr<D3DUtils::MeshGeometry> m_BoxGeometry;
	std::unique_ptr<D3DUtils::MeshGeometry> m_PyramidGeometry;
	std::unique_ptr<D3DUtils::MeshGeometry> m_GridGeometry;

private:
	float m_alpha = 0.25f * XM_PI;
	float m_beta = 0.25f * XM_PI;
	float m_radius = 5.0f;

	POINT m_LastMousePos;

	XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();
};

