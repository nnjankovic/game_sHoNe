#pragma once
#ifndef DRAWITEM_H_
#define DRAWITEM_H_

#include "Utility.h"
#include "D3D12Renderer.h"
#include "GameTimer.h"
#include "IRenderer.h"

#include <memory>

using namespace DirectX;
using namespace DirectX::PackedVector;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct TexturedVertex {
	TexturedVertex() {};
	TexturedVertex(float x, float y, float z, float tx, float ty) :
		Pos(x, y, z),
		TexC(tx, ty) {}

	XMFLOAT3 Pos;
	XMFLOAT2 TexC;
};

struct Texture {
	std::wstring name;
	std::wstring fileName;

	int index;

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
};

struct DrawItemProperties {
	//ComPtr<ID3DBlob> vertexShaderByteCode;
	//ComPtr<ID3DBlob> pixelShaderByteCode;
	//ComPtr<ID3D12PipelineState> pipelineStateObject;
	//std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;

	ObjectConstants objectConstants;
	MathHelper::PositionVector position;
	float xAngle = 0;

	Texture texture;

	ShaderType shaderType = ShaderType::PLAIN;

	unsigned int constantBufferIndex = 0;
	//bool isTextured = false;
};

struct GeometryData {
	std::vector<Vertex> vertices;
	std::vector<TexturedVertex> texVertices;
	std::vector<std::uint16_t> indices;

	ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
	UINT VertexBufferSize;
	UINT VertexByteStride;
	
	ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;
	ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;
	UINT IndexBufferSize;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;



	D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferSize;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferSize;

		return ibv;
	}
};

class DrawItem{
friend class D3D12Renderer;
public:
	DrawItem(std::shared_ptr<IRenderer> renderer) : m_renderer(renderer) {}
	virtual ~DrawItem() {};

	virtual void create() {
		//createShadersAndInputLayout();
		m_properties.constantBufferIndex = s_constantBuffer++;
		loadGeometry();
		//m_renderer->createPSO(*this);
	}

	virtual void Draw(XMMATRIX matrixStack);

	virtual void Update(const GameTimer& gt) {}

protected:
	//virtual bool createShadersAndInputLayout() = 0;
	virtual bool loadGeometry() = 0;
	virtual bool loadTexture() { return false; }

protected:
	GeometryData m_geometry;

	DrawItemProperties m_properties;

	std::shared_ptr<IRenderer> m_renderer;

	static unsigned int s_constantBuffer;
};

#endif // !DRAWITEM_H_

