#pragma once

#include <map>
#include <algorithm>
#include "Math/CMath.h"

typedef enum CActorType
{
	RIGID,
	SKELETAL
};

class CActor 
{
	public:
		~CActor();
		virtual void Update(double time);
		std::wstring GetName();
		virtual CActorType GetType() = 0;
		void SetScale(DirectX::XMFLOAT3 scale);
		DirectX::XMFLOAT3 GetScale();
		void SetRotation(DirectX::XMFLOAT4 rotation);
		DirectX::XMFLOAT4 GetRotation();
		void SetPosition(DirectX::XMFLOAT3 position);
		DirectX::XMFLOAT3 GetPosition();

	protected:
		CActor(std::wstring name, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 position);
		std::wstring name;
		XMFLOAT3 scale;
		XMFLOAT4 rotation;
		XMFLOAT3 position;
};
