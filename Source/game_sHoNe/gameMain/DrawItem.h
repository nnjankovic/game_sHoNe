#pragma once
#ifndef DRAWITEM_H_
#define DRAWITEM_H_





#include "Utility.h"
#include "D3D12Renderer.h"
#include "GameTimer.h"
#include "IRenderer.h"
#include "IDrawItemComponent.h"

#include <memory>

using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Renderer3D {
	class MoveOnInputDrawItemComponent;

	struct BasicVertex
	{
		XMFLOAT3 Pos;
		XMFLOAT4 Color;
	};

	struct TexturedVertex {
		TexturedVertex() {};
		TexturedVertex(float x, float y, float z, float tx, float ty) :
			Pos(x, y, z),
			TexC(tx, ty) {}
		TexturedVertex(float x, float y, float z, float tx, float ty, float nx, float ny, float nz) :
			Pos(x, y, z),
			TexC(tx, ty),
			Normal(nx, ny, nz) {}
		TexturedVertex(MathHelper::Position3 pos, MathHelper::Position2 texPos, MathHelper::Position3 normalPos) :
			Pos(pos.x, pos.y, pos.z),
			TexC(texPos.x, texPos.y),
			Normal(normalPos.x, normalPos.y, normalPos.z) {}


		XMFLOAT3 Pos;
		XMFLOAT2 TexC;
		XMFLOAT3 Normal;
	};

	struct Texture {
		std::wstring name;
		std::wstring fileName;

		int index;

		Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
	};

	struct Material {
		std::wstring name;
		int index = -1;
		MaterialConstants materialConstants;
	};

	enum class coordinateAxes {
		X,
		Y,
		Z
	};


	struct DrawItemProperties {
		ObjectConstants objectConstants;
		MathHelper::Position3 position;
		float xAngle = 0;
		float yAngle = 0;
		float zAngle = 0;
		
		float scaleFactor = 1;

		Texture texture;
		MathHelper::Position2 textureScaling;
		Material material;

		ShaderType shaderType = ShaderType::Plain;

		unsigned int constantBufferIndex = 0;
		//bool isTextured = false;
	};

	struct GeometryData {
		std::vector<BasicVertex> vertices;
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

	class DrawItem {
		friend class D3D12Renderer;
		friend class MoveOnInputDrawItemComponent;
	public:
		DrawItem(std::shared_ptr<IRenderer> renderer) : m_renderer(renderer) {}
		virtual ~DrawItem();

		virtual void create();

		void addComponent(std::shared_ptr<IDrawItemComponent> component) {
			m_components.push_back(component);
		}

		virtual void Draw(XMMATRIX matrixStack);

		virtual void Update(const GameTimer& gt) {}

		void changeShaderType(Renderer3D::ShaderType shaderType) {
			m_properties.shaderType = shaderType;
		}

	protected:
		//virtual bool createShadersAndInputLayout() = 0;
		virtual bool loadGeometry() = 0;
		virtual bool loadTexture() { return false; }
		virtual bool computeNormals();

	protected:
		GeometryData m_geometry;

		DrawItemProperties m_properties;

		std::shared_ptr<IRenderer> m_renderer;

		static unsigned int s_constantBuffer;

		std::vector<std::shared_ptr<IDrawItemComponent>> m_components;
	};

};
#endif // !DRAWITEM_H_

