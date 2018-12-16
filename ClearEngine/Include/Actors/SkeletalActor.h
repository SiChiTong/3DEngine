#pragma once

#include "CActor.h"
#include "Geometry/Models.h"
#include "Math/MathUtils.h"
#include "Geometry/GeometryTypes.h"

class SkeletalActor : public CActor
{
	public:
		SkeletalActor(std::wstring name, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 position, SkeletalModel* model);
		~SkeletalActor();
		void Update(double time, ID3D11DeviceContext1* devCon);
		SkeletalModel* GetModel();
		ModelSubset* GetModelSubset(int subsetIndex);
		CActorType GetType();
		float GetAnimMult();
		void SetAnimMult(float animMult);
		ModelAnimation* GetAnimation();
		void SetAnimation(ModelAnimation* animation);
		void SetRotation(DirectX::XMFLOAT4 rotation);
		DirectX::XMFLOAT4 GetRotation();
		void SetPosition(DirectX::XMFLOAT3 position);
		DirectX::XMFLOAT3 GetPosition();

	private:
		void UpdateModel(float deltaTime, ID3D11DeviceContext1* devCon);
		SkeletalModel* model;
		ModelAnimation* animation;
		float animMult;
		float currAnimTime;
		bool shouldAnimate;
		bool reverseAnim;
};
