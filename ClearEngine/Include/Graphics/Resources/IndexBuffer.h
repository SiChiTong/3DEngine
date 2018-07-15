#pragma once

#include "DXBuffer.h"

class IndexBuffer : public DXBuffer
{
public:
	static IndexBuffer* Create(ID3D11Device1* device, BUFFER_LIFETIME& lifetime, UINT numElements, DWORD* data = NULL);
	~IndexBuffer();
	void Update(ID3D11DeviceContext1* devCon, DWORD* data);
	void Bind(ID3D11DeviceContext1* devCon, UINT offset = 0);

private:
	IndexBuffer();
};

