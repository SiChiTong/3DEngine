#pragma once

#include "Common/DirectX.h"

class Camera 
{
public:
	Camera();
	Camera(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 up, DirectX::XMFLOAT3 target, float fov, float width, float height, float nearZ, float farZ);
   ~Camera();
	void SetPitch(float p);
	void SetYaw(float y);
	void MoveFB(float amount);
	void MoveRL(float amount);
	void MoveUD(float amount);
	void SetPosition(DirectX::XMFLOAT3 newPos);
	DirectX::XMFLOAT3 GetPosition() const;
	void SetTarget(DirectX::XMFLOAT3 newTarget);
	DirectX::XMFLOAT3 GetTarget() const;
	void SetUp(DirectX::XMFLOAT3 newUp);
	DirectX::XMFLOAT3 GetUp() const;
	DirectX::XMVECTOR GetForward();
	DirectX::XMMATRIX GetView();
	DirectX::XMMATRIX GetProjection();
	DirectX::XMMATRIX GetViewProjection();
	void SetFOV(float fov);
	float GetFOV() const;
	void SetNearZ(float nearZ);
	float GetNearZ() const;
	void SetFarZ(float farZ);
	float GetFarZ() const;
	void SetWidth(float width);
	float GetWidth() const;
	void SetHeight(float height);
	float GetHeight() const;

private:
	DirectX::XMFLOAT3 mPosition;
	DirectX::XMFLOAT3 mTarget;
	DirectX::XMFLOAT3 mUp;
	float width;
	float height;
	float fov;
	float nearZ;
	float farZ;
};
