#pragma once

#include "DXBuffer.h"
#include "DXShaderResourceView.h"

template<typename T>
class StructuredBuffer : public DXBuffer, public DXShaderResourceView
{
public:
	static StructuredBuffer<T>* Create(ID3D11Device1* device, BUFFER_TYPE& type, UINT numElements, T* data = NULL)
	{
		StructuredBuffer<T>* structuredBuffer = new StructuredBuffer<T>();

		structuredBuffer->elementSize = sizeof(T);
		structuredBuffer->elementCount = numElements;

		ZeroMemory(&structuredBuffer->bufferDesc, sizeof(D3D11_BUFFER_DESC));
		structuredBuffer->bufferDesc.Usage = type == DEFAULT ? D3D11_USAGE_DEFAULT : DYNAMIC ? D3D11_USAGE_DYNAMIC : IMMUTABLE ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_STAGING;
		structuredBuffer->bufferDesc.ByteWidth = structuredBuffer->elementSize * numElements;
		structuredBuffer->bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		structuredBuffer->bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		structuredBuffer->bufferDesc.CPUAccessFlags = type == DYNAMIC || type == STAGING_WRITE ? D3D11_CPU_ACCESS_WRITE : 0;
		structuredBuffer->bufferDesc.StructureByteStride = structuredBuffer->elementSize;

		structuredBuffer->buffer = CreateBuffer(device, structuredBuffer->bufferDesc, data);

		D3D11_BUFFER_SRV srvBuffer;
		srvBuffer.FirstElement = 0;
		srvBuffer.NumElements = numElements;

		structuredBuffer->shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;  
		structuredBuffer->shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		structuredBuffer->shaderResourceViewDesc.Buffer = srvBuffer;

		structuredBuffer->shaderResourceView = CreateShaderResourceView(device, structuredBuffer->shaderResourceViewDesc, structuredBuffer->buffer);

		return structuredBuffer;
	}
	~StructuredBuffer() {}
	void Update(ID3D11DeviceContext1* devCon, T* data)
	{
		this->DXBuffer::Update(devCon, data);
	}

private:
	StructuredBuffer() : DXBuffer(), DXShaderResourceView() {}
};


