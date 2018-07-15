#pragma once

#include "Graphics/Resources/DXBuffer.h"

template<typename T>
class VertexBuffer : public DXBuffer
{
public:
	static VertexBuffer<T>* Create(ID3D11Device1* device, BUFFER_LIFETIME& lifetime, UINT numElements, T* data = NULL) 
	{
		VertexBuffer<T>* vertexBuffer = new VertexBuffer<T>();

		vertexBuffer->elementCount = numElements;
		vertexBuffer->elementSize = sizeof(T);

		ZeroMemory(&vertexBuffer->bufferDesc, sizeof(D3D11_BUFFER_DESC));
		vertexBuffer->bufferDesc.Usage = lifetime == FOREVER ? D3D11_USAGE_IMMUTABLE : lifetime == LONG_LIVED ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;
		vertexBuffer->bufferDesc.ByteWidth = vertexBuffer->elementSize * numElements;
		vertexBuffer->bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBuffer->bufferDesc.MiscFlags = 0;
		vertexBuffer->bufferDesc.CPUAccessFlags = vertexBuffer->bufferDesc.Usage == D3D11_USAGE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;

		vertexBuffer->buffer = CreateBuffer(device, vertexBuffer->bufferDesc, data);
		
		return vertexBuffer; 
	}
	~VertexBuffer() {}
	void Bind(ID3D11DeviceContext1* devCon, UINT startSlot, UINT offset = 0)
	{
		devCon->IASetVertexBuffers(startSlot, 1, &buffer, &elementSize, &offset);
	}
	void Update(ID3D11DeviceContext1* devCon, T* data)
	{
		this->DXBuffer::Update(devCon, data);
	}
	T* GetDataPointer(ID3D11DeviceContext1* devCon, UINT subresourceIndex)
	{
		void* data = DXBuffer::GetDataPointer(devCon, subresourceIndex);
		if(data == NULL) 
		{
			return NULL;
		}
		return (T*)data;
	}

private:
	VertexBuffer() : DXBuffer() {}
};






