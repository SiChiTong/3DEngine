#include "Actors/SkeletalActor.h"
using namespace DirectX;

SkeletalActor::SkeletalActor(std::wstring name, XMFLOAT3 scale, XMFLOAT4 rotation, XMFLOAT3 position, SkeletalModel* model)
	: CActor(name, scale, rotation, position), model(model)
{	
	currAnimTime = 0;
	animation = 0;
	animMult = 1.0f;
	shouldAnimate = true;
}

SkeletalActor::~SkeletalActor() {}

void SkeletalActor::Update(double time, ID3D11DeviceContext1* devCon)
{
	this->CActor::Update(time);
	UpdateModel(time, devCon);
}

void SkeletalActor::UpdateModel(float deltaTime, ID3D11DeviceContext1* devCon)
{
	if (!animation)
	{
		return;
	}

	HRESULT hr;
	currAnimTime += (deltaTime * animMult);			// Update the current animation time

	if(currAnimTime > animation->totalAnimTime)
	{
		currAnimTime = 0.0f;
	}
	
	// Which frame are we on
	float currentFrame; 
	currentFrame = currAnimTime * animation->frameRate;
	int frame0;
	int frame1;
	frame0 = floorf( currentFrame );
	frame1 = frame0 + 1;
	// Make sure we don't go over the number of frames	
	if (frame0 == animation->numFrames - 1)
	{
		currAnimTime = 0.0f;
		frame1 = 0;
	}
	
	float interpolation = currentFrame - frame0;
	std::vector<ShaderJoint> interpolatedSkeleton;		// Create a frame skeleton to store the interpolated skeletons in

	ShaderJoint tempJoint;
	Joint joint0;
	Joint joint1;
	XMVECTOR joint0Orient;
	XMVECTOR joint1Orient;

	// Compute the interpolated skeleton
	for (int i = 0; i < animation->numJoints; ++i)
	{
		joint0 = animation->frameSkeleton[frame0][i];		// Get the i'th joint of frame0's skeleton
		joint1 = animation->frameSkeleton[frame1][i];		// Get the i'th joint of frame1's skeleton
		
		if(!MathUtils::Float3AreEqual(joint0.pos, joint1.pos) || 
			!MathUtils::Float4AreEqual(joint0.orientation, joint1.orientation))
		{
			//changeFlags[i] = true;
			tempJoint.parentID = joint0.parentID;											// Set the tempJoints parent id

			// Turn the two quaternions into XMVECTORs for easy computations
			joint0Orient = XMVectorSet(joint0.orientation.x, joint0.orientation.y, joint0.orientation.z, joint0.orientation.w);
			joint1Orient = XMVectorSet(joint1.orientation.x, joint1.orientation.y, joint1.orientation.z, joint1.orientation.w);

			// Interpolate positions
			tempJoint.pos.x = joint0.pos.x + (interpolation * (joint1.pos.x - joint0.pos.x));
			tempJoint.pos.y = joint0.pos.y + (interpolation * (joint1.pos.y - joint0.pos.y));
			tempJoint.pos.z = joint0.pos.z + (interpolation * (joint1.pos.z - joint0.pos.z));
			// Interpolate orientations using spherical interpolation (Slerp)
			XMStoreFloat4(&tempJoint.orientation, XMQuaternionSlerp(joint0Orient, joint1Orient, interpolation));

			interpolatedSkeleton.push_back(tempJoint);		// Push the joint back into our interpolated skeleton
		}
		else
		{
			tempJoint.parentID = joint1.parentID;

			tempJoint.pos = joint1.pos;

			tempJoint.orientation = joint1.orientation;

			interpolatedSkeleton.push_back(tempJoint);
		}
	}

	model->jointSB->Update(devCon, &interpolatedSkeleton[0]);
}

SkeletalModel* SkeletalActor::GetModel()
{
	return model;
}

ModelSubset* SkeletalActor::GetModelSubset(int subsetIndex)
{
	return &this->model->subsets.at(subsetIndex);
}

float SkeletalActor::GetAnimMult()
{
	return this->animMult;
}
		
void SkeletalActor::SetAnimMult(float animMult)
{
	this->animMult = animMult;
}

ModelAnimation* SkeletalActor::GetAnimation()
{
	return this->animation;
}

void SkeletalActor::SetAnimation(ModelAnimation* animation)
{
	this->animation = animation;
	this->currAnimTime = 0.0f;
}

CActorType SkeletalActor::GetType()
{
	return CActorType::SKELETAL;
}

void SkeletalActor::SetRotation(DirectX::XMFLOAT4 rotation)
{
	
}

XMFLOAT4 SkeletalActor::GetRotation()
{
	return XMFLOAT4(0, 0, 0, 1);
}

void SkeletalActor::SetPosition(DirectX::XMFLOAT3 position)
{
	
}

XMFLOAT3 SkeletalActor::GetPosition()
{
	return XMFLOAT3(0, 2, 0);
}

//void SkeletalActor::SetNewRotation(XMVECTOR newDirection)
//{
//float dot = XMVectorGetX(XMVector3Dot(direction.GetXMVector(), newDirection));

//if(1 - dot >= 0.000001f)
//{
//XMVECTOR axis = XMVector3Normalize(XMVector3Cross(direction.GetXMVector(), newDirection));
//XMVECTOR qauternion;
//if(!XMVector3Equal(axis, XMVectorZero()))
//{
//float angle = XMScalarACos(dot);
//qauternion = XMQuaternionRotationAxis(axis, angle);
//up.SetXMVector(XMVector3Rotate(up.GetXMVector(), qauternion));
//}
//else
//{
//qauternion = XMQuaternionRotationAxis(up.GetXMVector(), 3.1415925f);
//}
//rotation.SetXMVector(XMQuaternionMultiply(rotation.GetXMVector(), qauternion));
//}
//}
