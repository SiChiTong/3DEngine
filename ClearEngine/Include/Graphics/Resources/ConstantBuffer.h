#pragma once

#include "DXBuffer.h"

template<typename T>
class ConstantBuffer : public DXBuffer
{
public:
	static ConstantBuffer<T>* Create(ID3D11Device1* device, T* data = NULL)
	{
		ConstantBuffer<T>* constantBuffer = new ConstantBuffer();

		constantBuffer->elementCount = 1;

		ZeroMemory(&constantBuffer->bufferDesc, sizeof(D3D11_BUFFER_DESC));
		constantBuffer->bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBuffer->bufferDesc.ByteWidth = constantBuffer->elementSize = sizeof(T);
		constantBuffer->bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBuffer->bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBuffer->bufferDesc.MiscFlags = 0;

		constantBuffer->buffer = CreateBuffer(device, constantBuffer->bufferDesc, data);

		return constantBuffer;
	}
	~ConstantBuffer() {}
	void Update(ID3D11DeviceContext1* devCon, T* data)
	{
		this->DXBuffer::Update(devCon, data);
	}

private:
	ConstantBuffer() : DXBuffer() {}
};
