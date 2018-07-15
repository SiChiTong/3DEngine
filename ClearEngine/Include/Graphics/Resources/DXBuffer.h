#pragma once

#include "Common/DirectX.h"

class DXBuffer 
{
public:
	//Used in some buffer wrappers.
	enum BUFFER_LIFETIME
	{
		FOREVER, LONG_LIVED, TEMPORARY
	};

	//Used in some buffer wrappers.
	enum BUFFER_TYPE
	{
		DEFAULT, IMMUTABLE, DYNAMIC, STAGING_WRITE, STAGING_READ
	};
	
	static ID3D11Buffer* CreateBuffer(ID3D11Device1* device, D3D11_BUFFER_DESC& bufferDesc, const void* data);
	UINT GetByteWidth();
	UINT GetElementSize();
	UINT GetElementCount();
	D3D11_BUFFER_DESC GetBufferDescription();
	ID3D11Buffer* GetBuffer();
	void UnMap(ID3D11DeviceContext1* devCon, UINT subResourceIndex);

protected:
	DXBuffer();
	virtual ~DXBuffer();
	void Update(ID3D11DeviceContext1* devCon, const void* dataSrc);
	void* GetDataPointer(ID3D11DeviceContext1* devCon, UINT subresourceIndex);
	D3D11_BUFFER_DESC bufferDesc;
	UINT elementSize;
	UINT elementCount;
	ID3D11Buffer* buffer = 0;
};

