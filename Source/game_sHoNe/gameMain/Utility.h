#pragma once
#ifndef UTILITY_H_
#define UTILITY_H_

#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
#include "d3dx12.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

#include <Windows.h>
#include <DirectXMath.h>
#include <cstdint>

namespace MathHelper {
	struct PositionVector {
		float x;
		float y;
		float z;
	};

	static DirectX::XMFLOAT4X4 Identity4x4()
	{
		static DirectX::XMFLOAT4X4 I(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		return I;
	}
}

static UINT CalcConstantBufferByteSize(UINT byteSize)
{
	return (byteSize + 256) - byteSize % 256;
}

ComPtr<ID3DBlob> CompileShader(
	const std::wstring& filename,
	const D3D_SHADER_MACRO* defines,
	const std::string& entrypoint,
	const std::string& target);

template<typename T>
class upload_helper {
public:
	upload_helper(ID3D12Device* device, UINT elementCount, bool isConstantBuffer) : m_elementCount(elementCount) {
		if (!isConstantBuffer)
			m_elementSize = sizeof(T);
		else
			m_elementSize = CalcConstantBufferByteSize(sizeof(T)); //Constant bufer size is multiple of 256

		HRESULT hr = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(m_elementSize*elementCount),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_uploadBuffer));

		assert(!(FAILED(hr)));

		hr = m_uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedData));
		assert(!(FAILED(hr)));
	}

	void setData(const T& data, unsigned int index = 0) {
		assert(index < m_elementCount);
		memcpy(&m_MappedData[index*m_elementSize], &data, sizeof(T));
	}

	ID3D12Resource* Resource() const {
		return m_uploadBuffer.Get();
	}

	UINT getElementSize() {
		return m_elementSize;
	}

	~upload_helper() {
		if (m_uploadBuffer != nullptr)
			m_uploadBuffer->Unmap(0, nullptr);
		m_MappedData = nullptr;
	}
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_uploadBuffer;
	BYTE* m_MappedData = nullptr;

	unsigned int m_elementCount;
	uint16_t m_elementSize;
};

ComPtr<ID3D12Resource> CreateDefaultBuffer(
	ID3D12Device* device,
	ID3D12GraphicsCommandList* cmdList,
	const void* initData,
	UINT64 byteSize,
	Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

#endif // !UTILITY_H_

