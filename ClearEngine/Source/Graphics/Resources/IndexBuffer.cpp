#include "Graphics/Resources/IndexBuffer.h"

IndexBuffer* IndexBuffer::Create(ID3D11Device1* device, BUFFER_LIFETIME& lifetime, UINT numElements, DWORD* data) 
{
	IndexBuffer* indexBuffer = new IndexBuffer();

	indexBuffer->elementCount = numElements;
	indexBuffer->elementSize = sizeof(DWORD);

	ZeroMemory(&indexBuffer->bufferDesc, sizeof(D3D11_BUFFER_DESC));
	indexBuffer->bufferDesc.ByteWidth = indexBuffer->elementSize * numElements;
	indexBuffer->bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBuffer->bufferDesc.MiscFlags = 0;
	indexBuffer->bufferDesc.Usage = lifetime == FOREVER ? D3D11_USAGE_IMMUTABLE : lifetime == LONG_LIVED ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;
	indexBuffer->bufferDesc.CPUAccessFlags = indexBuffer->bufferDesc.Usage == D3D11_USAGE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;

	indexBuffer->buffer = CreateBuffer(device, indexBuffer->bufferDesc, data);

	return indexBuffer;
}

IndexBuffer::IndexBuffer() : DXBuffer()  {}

IndexBuffer::~IndexBuffer() {}

void IndexBuffer::Bind(ID3D11DeviceContext1* devCon, UINT offset)
{
	devCon->IASetIndexBuffer(this->buffer, DXGI_FORMAT_R32_UINT, offset);
}

void IndexBuffer::Update(ID3D11DeviceContext1* devCon, DWORD* data)
{
	this->DXBuffer::Update(devCon, data);
}