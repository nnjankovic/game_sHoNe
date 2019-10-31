#pragma once

#include "Utils.h"

template<typename T>
class UploadBuffer
{
public:
	UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer) :
	m_isConstantBuffer(isConstantBuffer),
	m_elementSize(0)
	{
		if (!m_isConstantBuffer)
			m_elementSize = sizeof(T);
		else
			m_elementSize = D3DUtils::CalcConstantBufferByteSize(sizeof(T));

		HRESULT hr = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(m_elementSize*elementCount),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_UploadBuffer));

		assert(!(FAILED(hr)));

		hr = m_UploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedData));
		assert(!(FAILED(hr)));
	}

	UploadBuffer(const UploadBuffer& rhs) = delete;
	UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
	UploadBuffer(const UploadBuffer&& rhs) = delete;
	UploadBuffer& operator=(const UploadBuffer&& rhs) = delete;

	~UploadBuffer() {
		if (nullptr != m_UploadBuffer)
			m_UploadBuffer->Unmap(0, nullptr);

		m_MappedData = nullptr;
	}

	void CopyData(int index, const T& data) {
		memcpy(&m_MappedData[index*m_elementSize], &data, sizeof(T));
	}

	ID3D12Resource* Resource() const {
		return m_UploadBuffer.Get();
	}

	UINT getElementSize() {
		return m_elementSize;
	}

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_UploadBuffer;
	BYTE* m_MappedData = nullptr;

	bool m_isConstantBuffer;
	UINT m_elementSize;

};
