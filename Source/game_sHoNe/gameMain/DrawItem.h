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
	Vertex(float x, float y, float z,
		float m, float n
	) : Pos(x, y, z), TexC(m, n), hasTexture(1) {}

	Vertex(XMFLOAT3 pos, XMFLOAT4 color) : Pos(pos), Color(color), hasTexture(0) {}

	Vertex() {}

	XMFLOAT3 Pos;
	XMFLOAT4 Color;

	XMFLOAT2 TexC;
	int hasTexture; //0 NO TEXTURE;  
};

struct Texture {
	std::wstring name;
	std::wstring fileName;

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
};

struct DrawItemProperties {
	ComPtr<ID3DBlob> vertexShaderByteCode;
	ComPtr<ID3DBlob> pixelShaderByteCode;
	ComPtr<ID3D12PipelineState> pipelineStateObject;

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;

	ObjectConstants objectConstants;
	MathHelper::PositionVector position;

	Texture texture;

	bool isTextured = false;
};

struct GeometryData {
	std::vector<Vertex> vertices;
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
		createShadersAndInputLayout();
		loadGeometry();
		m_renderer->createPSO(*this);
	}

	virtual void Draw(XMMATRIX matrixStack);

	virtual void Update(const GameTimer& gt) {}

protected:
	virtual bool createShadersAndInputLayout() = 0;
	virtual bool loadGeometry() = 0;
	virtual bool loadTexture() { return false; }

protected:
	GeometryData m_geometry;

	DrawItemProperties m_properties;

	std::shared_ptr<IRenderer> m_renderer;
};

#endif // !DRAWITEM_H_

