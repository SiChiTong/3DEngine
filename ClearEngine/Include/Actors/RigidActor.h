#pragma once

#include "CActor.h"

class RigidActor : public CActor
{
	public:
		virtual ~RigidActor();
		virtual void Update(double time);
		CActorType GetType();
		void SetScale(DirectX::XMFLOAT3 scale);
		void SetRotation(DirectX::XMFLOAT4 rotation);
		void SetPosition(DirectX::XMFLOAT3 position);

	protected:
		RigidActor(std::wstring name, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 position);
};

