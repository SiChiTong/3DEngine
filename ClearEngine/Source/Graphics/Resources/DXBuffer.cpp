#include "Graphics/Resources/DXBuffer.h"

DXBuffer::DXBuffer() {}

DXBuffer::~DXBuffer()
{
	buffer->Release();
}

ID3D11Buffer* DXBuffer::CreateBuffer(ID3D11Device1* device, D3D11_BUFFER_DESC& bufferDesc, const void* data)
{
	//Create buffer using above description and the address of the above buffer declaration.
	ID3D11Buffer* newBuffer;
	if(data != NULL)
	{
		D3D11_SUBRESOURCE_DATA pInitialData;
		pInitialData.pSysMem = data;
		device->CreateBuffer(&bufferDesc, &pInitialData, &newBuffer);
	}
	else
	{
		device->CreateBuffer(&bufferDesc, NULL, &newBuffer);
	}
	
	return newBuffer;
}

//Updates entire buffer
void DXBuffer::Update(ID3D11DeviceContext1* devCon, const void* dataSrc)
{
	if(bufferDesc.Usage == D3D11_USAGE_DYNAMIC)
	{
		D3D11_MAPPED_SUBRESOURCE mappedBuff;
		HRESULT hr = devCon->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuff);
		// Copy the data into the buffer.
		memcpy(mappedBuff.pData, dataSrc, bufferDesc.ByteWidth);
		devCon->Unmap(buffer, 0);
	}
	else 
	{
		devCon->UpdateSubresource( buffer, 0, NULL, dataSrc, 0, 0 );
	}
}

//Returns pointer to buffer data but only for dynamic buffers. Using with other buffers will return NULL.
void* DXBuffer::GetDataPointer(ID3D11DeviceContext1* devCon,  UINT subresourceIndex)
{
	if(bufferDesc.Usage == D3D11_USAGE_DYNAMIC)
	{
		D3D11_MAPPED_SUBRESOURCE mappedBuff;
		HRESULT hr = devCon->Map(buffer, subresourceIndex, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuff);
		return mappedBuff.pData;
	}
	return NULL;
}

void DXBuffer::UnMap(ID3D11DeviceContext1* devCon,  UINT subresourceIndex)
{
	if(bufferDesc.Usage == D3D11_USAGE_DYNAMIC)
	{
		devCon->Unmap(buffer, subresourceIndex);
	}
}

UINT DXBuffer::GetByteWidth()
{
	return bufferDesc.ByteWidth;
}

UINT DXBuffer::GetElementSize()
{
	return elementSize;
}

UINT DXBuffer::GetElementCount()
{
	return elementCount;	
}
	
D3D11_BUFFER_DESC DXBuffer::GetBufferDescription()
{
	return bufferDesc;
}

ID3D11Buffer* DXBuffer::GetBuffer()
{
	return buffer;
}


