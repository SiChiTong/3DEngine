#include "Graphics/Camera/Camera.h"
#include "Math/SIMDConversionUtils.h"
#include "Math/MathUtils.h"

using namespace DirectX;

Camera::Camera()
{
	//Set default values.
	//mPosition = XMFLOAT3(0, 0, 0);
	//mUp = XMFLOAT3(0, 1, 0);
	//mTarget = XMFLOAT3(0, 0, 100);
	//fov = 0.4f*3.14f;
	//width = 1920;
	//height = 1080;
	//nearZ = 1;
	//farZ = 4000;
	//width = 1920;
	//height = 1080;
	
	Camera(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 1, 0), XMFLOAT3(0, 0, 100), 0.4f*3.14f, 1920, 1080, 1, 4000);
}

Camera::Camera(XMFLOAT3 position, XMFLOAT3 up, XMFLOAT3 target, float fov, float width, float height, float nearZ, float farZ)
: mPosition(position), mUp(up), mTarget(target), fov(fov), width(width), height(height), nearZ(nearZ), farZ(farZ)
{
	
}

Camera::~Camera()
{
	
}

void Camera::SetYaw(float yaw)
{
	XMVECTOR vPosition = FV(mPosition);
	XMVECTOR forwardLength = FV(mTarget) - vPosition;
	XMVECTOR rot = XMQuaternionRotationAxis(FV(XMFLOAT3(0, 1, 0)), yaw);
	mUp = VF3(XMVector3Rotate(FV(mUp), rot));
	mTarget = VF3(XMVector3Rotate(forwardLength, rot) + vPosition);
}

void Camera::SetPitch(float pitch)
{
	XMVECTOR vPosition = FV(mPosition);
	XMVECTOR forwardLength = FV(mTarget) - vPosition;
	XMVECTOR forward = XMVector3Normalize(forwardLength);
	XMVECTOR vUp = FV(mUp);
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(forward, vUp));
	XMVECTOR rot = XMQuaternionRotationAxis(right, pitch);
	mUp = VF3(XMVector3Rotate(vUp, rot));
	mTarget = VF3(XMVector3Rotate(forwardLength, rot) + vPosition);
}

void Camera::MoveFB(float amount)
{
	XMVECTOR vPosition = FV(mPosition);
	XMVECTOR vTarget = FV(mTarget);
	XMVECTOR forward = XMVector3Normalize(vTarget - vPosition);
	vPosition = vPosition + forward * amount;
	mPosition = VF3(vPosition);
	mTarget = VF3(vPosition + forward * 100);
}

void Camera::MoveRL(float amount)
{
	XMVECTOR vPosition = FV(mPosition);
	XMVECTOR vTarget = FV(mTarget);
	XMVECTOR forward = XMVector3Normalize(vTarget - vPosition);
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(FV(mUp), forward));
	vPosition = vPosition + right * amount;
	mPosition = VF3(vPosition);
	mTarget = VF3(vPosition + forward * 100);
}

void Camera::MoveUD(float amount)
{
	XMVECTOR vPosition = FV(mPosition);
	XMVECTOR vTarget = FV(mTarget);
	XMVECTOR forward = XMVector3Normalize(vTarget - vPosition);
	vPosition = vPosition + FV(mUp) * amount;
	mPosition = VF3(vPosition);
	mTarget = VF3(vPosition + forward * 100);
}

void Camera::SetPosition(XMFLOAT3 position)
{
	XMVECTOR vOldPosition = FV(mPosition);
	XMVECTOR vPosition = FV(position);
	XMVECTOR vTarget = FV(mTarget);
	XMVECTOR forward = XMVector3Normalize(vTarget - vOldPosition);
	mPosition = position;
	mTarget = VF3(vPosition + forward * XMVector3Length(vTarget - vOldPosition));
}

XMFLOAT3 Camera::GetPosition() const
{
	return mPosition;
}

void Camera::SetTarget(XMFLOAT3 target)
{
	XMVECTOR vPosition = FV(mPosition);
	XMVECTOR vTarget = FV(target);
	//Make sure position and target are not the same
	if (XMVector3Equal(vPosition, vTarget))
	{
		return;
	}
	XMVECTOR currentForward = XMVector3Normalize(FV(mTarget) - vPosition);
	XMVECTOR newForward = XMVector3Normalize(vTarget - vPosition);

	float angle = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(currentForward, newForward)));

	XMVECTOR rot;
	if (angle != 0.0f && angle != 180.0f && angle != 360.0f)
	{
		XMVECTOR axis = XMVector3Normalize(XMVector3Cross(currentForward, newForward));
		rot = XMQuaternionRotationAxis(axis, XMConvertToRadians(angle));
		mUp = VF3(XMVector3Rotate(FV(mUp), rot));
	}
	mTarget = target;
}

XMFLOAT3 Camera::GetTarget() const
{
	return mTarget;
}

void Camera::SetUp(XMFLOAT3 up)
{
	XMVECTOR currentUp = FV(mUp);
	XMVECTOR newUp = FV(up);

	float angle = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(currentUp, newUp)));

	XMVECTOR rot;
	if (angle != 0.0f && angle != 180.0f && angle != 360.0f)
	{
		XMVECTOR axis = XMVector3Normalize(XMVector3Cross(currentUp, newUp));
		rot = XMQuaternionRotationAxis(axis, XMConvertToRadians(angle));
		mTarget = VF3(XMVector3Rotate(FV(mTarget), rot));
	}

	mUp = up;
}

XMFLOAT3 Camera::GetUp() const
{
	return mUp;
}

XMVECTOR Camera::GetForward() 
{
	return XMVector3Normalize(FV(mTarget) - FV(mPosition));
}

XMMATRIX Camera::GetView() 
{
	return XMMatrixLookAtLH(FV(mPosition), FV(mTarget), FV(mUp));
}

XMMATRIX Camera::GetProjection() 
{
	return XMMatrixPerspectiveFovLH(0.4f*3.14f, width / height, nearZ, farZ);
}

XMMATRIX Camera::GetViewProjection()
{
	return GetView() * GetProjection();
}

void Camera::SetFOV(float fov)
{
	this->fov = fov;
}

float Camera::GetFOV() const
{
	return fov;
}

void Camera::SetNearZ(float nearZ)
{
	this->nearZ = nearZ;
}

float Camera::GetNearZ() const
{
	return nearZ;
}

void Camera::SetFarZ(float farZ)
{
	this->farZ = farZ;
}

float Camera::GetFarZ() const
{
	return farZ;
}

void Camera::SetWidth(float width)
{
	this->width = width;
}

float Camera::GetWidth() const
{
	return width;
}

void Camera::SetHeight(float height)
{
	this->height = height;
}

float Camera::GetHeight() const
{
	return height;
}



