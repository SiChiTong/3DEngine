#include "Geometry/GeometryUtils.h"
#include "Common/GeneralUtils.h"
#include "Math/SIMDConversionUtils.h"
#include "Math/MathUtils.h"

using namespace DirectX;

void GeometryUtils::ComputeRigidModelData(RigidModel& model)
{
	std::vector<XMFLOAT3> tempNormal;

	//normalized and unnormalized normals
	XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

	//tangent stuff
	std::vector<XMFLOAT3> tempTangent;
	XMFLOAT3 tangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float tcU1, tcV1, tcU2, tcV2;

	//Two edges of our triangle
	XMFLOAT3 edge1, edge2;

	UINT numVertices = model.vertices.size();
	UINT numSubsets = model.subsets.size();
	UINT numTriangles = model.indices.size() / 3;

	int index;

	//Compute face normals
	for (int i = 0; i < numTriangles; ++i)
	{
		index = i * 3;

		//Get the vector describing one edge of our triangle (edge 0,1)
		edge1.x = model.vertices[model.indices[(index)+1]].pos.x - model.vertices[model.indices[(index)]].pos.x;
		edge1.y = model.vertices[model.indices[(index)+1]].pos.y - model.vertices[model.indices[(index)]].pos.y;
		edge1.z = model.vertices[model.indices[(index)+1]].pos.z - model.vertices[model.indices[(index)]].pos.z;

		//Get the vector describing another edge of our triangle (edge 0,2)
		edge2.x = model.vertices[model.indices[(index)+2]].pos.x - model.vertices[model.indices[(index)]].pos.x;
		edge2.y = model.vertices[model.indices[(index)+2]].pos.y - model.vertices[model.indices[(index)]].pos.y;
		edge2.z = model.vertices[model.indices[(index)+2]].pos.z - model.vertices[model.indices[(index)]].pos.z;

		//Cross multiply the two edge vectors to get the un-normalized face normal
		unnormalized = MathUtils::GetCrossProduct(edge1, edge2);

		//Save unormalized normal (for normal averaging)
		tempNormal.push_back(unnormalized);

		//Find first texture coordinate edge 2d vector
		tcU1 = model.vertices[model.indices[(index)+1]].texCoord.x - model.vertices[model.indices[(index)]].texCoord.x;
		tcV1 = model.vertices[model.indices[(index)+1]].texCoord.y - model.vertices[model.indices[(index)]].texCoord.y;

		//Find second texture coordinate edge 2d vector
		tcU2 = model.vertices[model.indices[(index)+2]].texCoord.x - model.vertices[model.indices[(index)]].texCoord.x;
		tcV2 = model.vertices[model.indices[(index)+2]].texCoord.y - model.vertices[model.indices[(index)]].texCoord.y;

		//Find tangent using both tex coord edges and position edges
		float r = 1.0f / (tcU1 * tcV2 - tcU2 * tcV1);
		tangent.x = (tcV2 * edge1.x - tcV1 * edge2.x) * r;
		tangent.y = (tcV2 * edge1.y - tcV1 * edge2.y) * r;
		tangent.z = (tcV2 * edge1.z - tcV1 * edge2.z) * r;

		tempTangent.push_back(tangent);
	}

	//Compute vertex normals (normal Averaging)
	XMFLOAT3 normalSum;
	XMFLOAT3 tangentSum;
	int facesUsing;

	//Go through each vertex
	for (int i = 0; i < numVertices; ++i)
	{
		//Clear normalSum, tangentSum and facesUsing for next vertex
		normalSum = XMFLOAT3(0.0f, 0.0f, 0.0f);
		tangentSum = XMFLOAT3(0.0f, 0.0f, 0.0f);
		facesUsing = 0;

		//Check which triangles use this vertex
		for (int j = 0; j < numTriangles; ++j)
		{
			if (model.indices[j * 3] == i ||
				model.indices[(j * 3) + 1] == i ||
				model.indices[(j * 3) + 2] == i)
			{
				//If a face is using the vertex, add the unormalized face normal to the normalSum
				normalSum.x += tempNormal[j].x;
				normalSum.y += tempNormal[j].y;
				normalSum.z += tempNormal[j].z;

				//sum up face tangents using this vertex
				tangentSum.x += tempTangent[j].x;
				tangentSum.y += tempTangent[j].y;
				tangentSum.z += tempTangent[j].z;

				facesUsing++;
			}
		}

		float scale = facesUsing == 0 ? 1 : 1.0f / facesUsing;
		//Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
		normalSum = MathUtils::ScaleFloat3(normalSum, scale);

		tangentSum = MathUtils::ScaleFloat3(tangentSum, scale);

		//Normalize the normalSum vector and tangent and assign
		model.vertices[i].normal = MathUtils::NormalizeFloat3(normalSum);

		model.vertices[i].tangent = MathUtils::NormalizeFloat3(tangentSum);
	}
	CreateRigidModelAABB(model);
}

void GeometryUtils::CreateRigidModelAABB(RigidModel& model)
{
	for (int i = 0; i < model.subsets.size(); i++)
	{
		CreateModelSubsetAABB(model, i);
	}
}

void GeometryUtils::CreateModelSubsetAABB(RigidModel& model, int subsetIndex)
{
	ModelSubset& subset = model.subsets[subsetIndex];
	int vertStartIndex = model.indices[subset.vertStartIndex];
	int vertexCount = model.indices[subset.vertStartIndex + subset.indexCount - 1] - vertStartIndex + 1;
	subset.aabb.min = model.vertices[vertStartIndex].pos;
	subset.aabb.max = model.vertices[vertStartIndex].pos;

	//Go through each vertex
	for (int i = vertStartIndex; i < vertexCount; ++i)
	{
		//Update subset's bounding box.
		XMFLOAT3 position = model.vertices[i].pos;
		if (position.x < 0)
		{
			std::cout << position.x;
		}
		//Update min values.
		subset.aabb.min.x = position.x < subset.aabb.min.x ? position.x : subset.aabb.min.x;
		subset.aabb.min.y = position.y < subset.aabb.min.y ? position.y : subset.aabb.min.y;
		subset.aabb.min.z = position.z < subset.aabb.min.z ? position.z : subset.aabb.min.z;
		//Update max values.
		subset.aabb.max.x = position.x > subset.aabb.max.x ? position.x : subset.aabb.max.x;
		subset.aabb.max.y = position.y > subset.aabb.max.y ? position.y : subset.aabb.max.y;
		subset.aabb.max.z = position.z > subset.aabb.max.z ? position.z : subset.aabb.max.z;
	}
}

void GeometryUtils::ComputeSkeletalModelData(SkeletalModel& model)
{
	std::vector<XMFLOAT3> positions(model.vertices.size());

	for (int i = 0; i < model.vertices.size(); ++i)
	{
		//*** find each vertex's position using the joints and weights ***//
		AnimVertex tempVert = model.vertices[i];

		// Make sure the vertex's pos is cleared first
		XMFLOAT3 pos = XMFLOAT3(0, 0, 0);
		// Sum up the joints and weights information to get vertex's position
		Weight tempWeight;
		Joint tempJoint;
		XMFLOAT4 tempJointOrientation;
		XMFLOAT4 tempWeightPos;
		XMFLOAT4 tempJointOrientationConjugate;
		XMFLOAT4 rotatedPoint;

		for (int j = 0; j < tempVert.WeightCount; ++j)
		{
			tempWeight = model.weights[tempVert.StartWeight + j];
			tempJoint = model.joints[tempWeight.jointID];

			// Convert joint orientation and weight pos to vectors for easier computation
			// When converting a 3d vector to a quaternion, you should put 0 for "w", and
			// When converting a quaternion to a 3d vector, you can just ignore the "w"
			tempJointOrientation = XMFLOAT4(tempJoint.orientation.x, tempJoint.orientation.y, tempJoint.orientation.z, tempJoint.orientation.w);
			tempWeightPos = XMFLOAT4(tempWeight.pos.x, tempWeight.pos.y, tempWeight.pos.z, 0.0f);

			// We will need to use the conjugate of the joint orientation quaternion
			// To get the conjugate of a quaternion, all you have to do is inverse the x, y, and z
			tempJointOrientationConjugate = XMFLOAT4(-tempJoint.orientation.x, -tempJoint.orientation.y, -tempJoint.orientation.z, tempJoint.orientation.w);

			// Calculate vertex position (in joint space, eg. rotate the point around (0,0,0)) for this weight using the joint orientation quaternion and its conjugate
			// We can rotate a point using a quaternion with the equation "rotatedPoint = quaternion * point * quaternionConjugate"

			rotatedPoint = MathUtils::GetQuaternionProduct(MathUtils::GetQuaternionProduct(tempJointOrientation, tempWeightPos), tempJointOrientationConjugate);

			// Now move the vertices position from joint space (0,0,0) to the joints position in world space, taking the weights bias into account
			// The weight bias is used because multiple weights might have an effect on the vertices final position. Each weight is attached to one joint.
			pos.x += (tempJoint.pos.x + rotatedPoint.x) * tempWeight.bias;
			pos.y += (tempJoint.pos.y + rotatedPoint.y) * tempWeight.bias;
			pos.z += (tempJoint.pos.z + rotatedPoint.z) * tempWeight.bias;	
		}

		positions[i] = pos;
	}

	//Tangent variables
	std::vector<XMFLOAT3> tempTangent;
	XMFLOAT3 tangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float tcU1, tcV1, tcU2, tcV2;

	//*** Calculate vertex normals using normal averaging ***///
	std::vector<XMFLOAT3> tempNormal;

	//normalized and unnormalized normals
	XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

	//Two edges of our triangle
	XMFLOAT3 edge1, edge2;;

	int index;

	UINT numTriangles = model.indices.size() / 3;

	//Compute face normals
	for (int i = 0; i < numTriangles; ++i)
	{
		index = i * 3;

		//Get the vector describing one edge of our triangle (edge 0,1)
		edge1.x = positions[model.indices[(index)+1]].x - positions[model.indices[(index)]].x;
		edge1.y = positions[model.indices[(index)+1]].y - positions[model.indices[(index)]].y;
		edge1.z = positions[model.indices[(index)+1]].z - positions[model.indices[(index)]].z;

		//Get the vector describing another edge of our triangle (edge 0,2)
		edge2.x = positions[model.indices[(index)+2]].x - positions[model.indices[(index)]].x;
		edge2.y = positions[model.indices[(index)+2]].y - positions[model.indices[(index)]].y;
		edge2.z = positions[model.indices[(index)+2]].z - positions[model.indices[(index)]].z;

		//Cross multiply the two edge vectors to get the un-normalized face normal
		unnormalized = MathUtils::GetCrossProduct(edge1, edge2);

		tempNormal.push_back(unnormalized);

		//Find first texture coordinate edge 2d vector
		tcU1 = model.vertices[model.indices[(index)+1]].texCoord.x - model.vertices[model.indices[(index)]].texCoord.x;
		tcV1 = model.vertices[model.indices[(index)+1]].texCoord.y - model.vertices[model.indices[(index)]].texCoord.y;

		//Find second texture coordinate edge 2d vector
		tcU2 = model.vertices[model.indices[(index)+2]].texCoord.x - model.vertices[model.indices[(index)]].texCoord.x;
		tcV2 = model.vertices[model.indices[(index)+2]].texCoord.y - model.vertices[model.indices[(index)]].texCoord.y;

		//Find tangent using both tex coord edges and position edges
		float r = 1.0f / (tcU1 * tcV2 - tcU2 * tcV1);
		tangent.x = (tcV2 * edge1.x - tcV1 * edge2.x) * r;
		tangent.y = (tcV2 * edge1.y - tcV1 * edge2.y) * r;
		tangent.z = (tcV2 * edge1.z - tcV1 * edge2.z) * r;

		tempTangent.push_back(tangent);
	}

	//Compute vertex normals (normal Averaging)
	XMFLOAT3 normalSum;
	XMFLOAT3 tangentSum;
	int facesUsing;

	//Go through each vertex
	for (int i = 0; i < model.vertices.size(); ++i)
	{
		//Clear normalSum, tangent and  facesUsing for next vertex
		normalSum = XMFLOAT3(0.0f, 0.0f, 0.0f);
		tangentSum = XMFLOAT3(0.0f, 0.0f, 0.0f);
		facesUsing = 0;
		//Check which triangles use this vertex
		for (int j = 0; j < numTriangles; ++j)
		{
			if (model.indices[j * 3] == i ||
				model.indices[(j * 3) + 1] == i ||
				model.indices[(j * 3) + 2] == i)
			{
				normalSum.x += tempNormal[j].x;
				normalSum.y += tempNormal[j].y;
				normalSum.z += tempNormal[j].z;

				//We can reuse tX, tY, tZ to sum up tangents
				tangentSum.x + tempTangent[j].x;
				tangentSum.y + tempTangent[j].y;
				tangentSum.z + tempTangent[j].z;

				facesUsing++;
			}
		}

		float scale = facesUsing == 0 ? 1 : 1.0f / facesUsing;

		//Get the actual normal by dividing the normalSum by the number of faces sharing the vertex (getting the average normal)
		normalSum = MathUtils::ScaleFloat3(normalSum, scale);

		//Get the actual tangent by dividing the tangentSum by the number of faces sharing the vertex (getting the average tangent)
		tangentSum = MathUtils::ScaleFloat3(tangentSum, scale);

		//Normalize the normalSum vector and tangent

		//normalSum is now the normal of the vertex in joint space.
		normalSum = MathUtils::NormalizeFloat3(normalSum);

		//tangentSum is now the tangent of the vertex in joint space.
		tangentSum = MathUtils::NormalizeFloat3(tangentSum);

		// Create the joint space normal for easy normal calculations in animation
		AnimVertex tempVert = model.vertices[i];						// Get the current vertex

		XMFLOAT4 normal = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		XMFLOAT4 tangent = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		Joint tempJoint;
		XMFLOAT4 jointOrientation;
		XMFLOAT4 jointOrientationConjugate;

		for (int k = 0; k < tempVert.WeightCount; k++)				// Loop through each of the vertices weights
		{
			tempJoint = model.joints[model.weights[tempVert.StartWeight + k].jointID];	// Get the joints orientation
			jointOrientation = XMFLOAT4(tempJoint.orientation.x, tempJoint.orientation.y, tempJoint.orientation.z, tempJoint.orientation.w);
			jointOrientationConjugate = XMFLOAT4(-tempJoint.orientation.x, -tempJoint.orientation.y, -tempJoint.orientation.z, tempJoint.orientation.w);

			// Calculate normal based off of the joint's orientation (turn into joint space)
			normal = MathUtils::GetQuaternionProduct(MathUtils::GetQuaternionProduct(jointOrientationConjugate, XMFLOAT4(normalSum.x, normalSum.y, normalSum.z, 0.0f)), jointOrientation);

			// Store the normalized quaternion into our weights normal
			model.weights[tempVert.StartWeight + k].normal = MathUtils::NormalizeFloat3(XMFLOAT3(normal.x, normal.y, normal.z));

			// Calculate tangent based off of the joint's orientation (turn into joint space)
			tangent = MathUtils::GetQuaternionProduct(MathUtils::GetQuaternionProduct(jointOrientationConjugate, XMFLOAT4(tangentSum.x, tangentSum.y, tangentSum.z, 0.0f)), jointOrientation);

			// Store the normalized quaternion into our weight's tangent
			model.weights[tempVert.StartWeight + k].tangent = MathUtils::NormalizeFloat3(XMFLOAT3(tangent.x, tangent.y, tangent.z));
		}
	}
}

void GeometryUtils::CreateRigidModelObjects(ID3D11Device1* device, RigidModel& model)
{
	HRESULT hr;
	DXBuffer::BUFFER_LIFETIME bufferTime = DXBuffer::LONG_LIVED;
	//Create index buffer
	UINT numTriangles = model.indices.size() / 3;
	model.indexBuffer = IndexBuffer::Create(device, bufferTime, numTriangles * 3, &model.indices[0]);

	//Create vertex Buffer
	model.vertexBuffer = VertexBuffer<DVertex>::Create(device, bufferTime, model.vertices.size(), &model.vertices[0]);

	std::vector<XMFLOAT3> positions;
	UINT totalVertices = 0;
	for (int i = 0; i < model.subsets.size(); ++i)
	{
		ModelSubset& subset = model.subsets[i];
		for (int j = totalVertices; j < totalVertices + subset.vertexCount; ++j)
		{
			subset.positions.push_back(model.vertices[j].pos);
			positions.push_back(model.vertices[j].pos);
		}
		//Calculate bounding sphere for the model subset.
		CreateBoundingSphere(subset.positions, subset.boundingSphere);
		totalVertices += subset.positions.size();
	}
	//Calculate bounding sphere for the model.
	CreateBoundingSphere(positions, model.boundingSphere);
}

void GeometryUtils::CreateSkeletalModelObjects(ID3D11Device1* device, SkeletalModel& model)
{
	HRESULT hr;
	DXBuffer::BUFFER_LIFETIME bufferTime = DXBuffer::LONG_LIVED;
	//Create index buffer
	UINT numTriangles = model.indices.size() / 3;
	model.indexBuffer = IndexBuffer::Create(device, bufferTime, numTriangles * 3, &model.indices[0]);

	//Create vertex Buffer
	model.vertexBuffer = VertexBuffer<AnimVertex>::Create(device, bufferTime, model.vertices.size(), &model.vertices[0]);

	std::vector<ShaderJoint> shaderJoints;
	ShaderJoint shaderJoint;
	Joint joint;
	for (int i = 0; i < model.joints.size(); ++i)
	{
		joint = model.joints[i];
		shaderJoint.pos = joint.pos;
		shaderJoint.orientation = joint.orientation;
		shaderJoint.parentID = joint.parentID;
		if (shaderJoint.parentID != -1)
		{
			ShaderJoint& parentJoint = shaderJoints[shaderJoint.parentID];
			XMVECTOR parentOrientation = FV(parentJoint.orientation);
			shaderJoint.orientation = VF4(XMQuaternionNormalize(XMQuaternionMultiply(parentOrientation, FV(shaderJoint.orientation))));
			shaderJoint.pos = VF3(XMQuaternionMultiply(XMQuaternionMultiply(parentOrientation, FV(shaderJoint.pos)), XMQuaternionConjugate(parentOrientation)));
			shaderJoint.pos.x += parentJoint.pos.x;
			shaderJoint.pos.y += parentJoint.pos.y;
			shaderJoint.pos.z += parentJoint.pos.z;
		}
		shaderJoints.push_back(shaderJoint);
	}

	DXBuffer::BUFFER_TYPE jointBufferType = DXBuffer::DYNAMIC;
	DXBuffer::BUFFER_TYPE weightBufferType = DXBuffer::DEFAULT;
	model.jointSB = StructuredBuffer<ShaderJoint>::Create(device, jointBufferType, model.joints.size(), &shaderJoints[0]);
	model.weightSB = StructuredBuffer<Weight>::Create(device, weightBufferType, model.weights.size(), &model.weights[0]);

	std::vector<XMFLOAT3> positions;
	UINT totalVertices = 0;
	//Calculate local positions based off of the default pose.
	for (int i = 0; i < model.subsets.size(); ++i)
	{
		ModelSubset& subset = model.subsets[i];
		for (int j = totalVertices; j < totalVertices + subset.vertexCount; ++j)
		{
			XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);
			AnimVertex& vertex = model.vertices[j];
			for (int k = vertex.StartWeight; k < vertex.WeightCount; ++k)
			{
				Weight& weight = model.weights[k];
				ShaderJoint& joint = shaderJoints[weight.jointID];
				XMVECTOR orientation = FV(joint.orientation);

				XMFLOAT3 weightPos = weight.pos;
				weightPos = VF3(XMQuaternionMultiply(XMQuaternionMultiply(orientation, FV(weightPos)), XMQuaternionConjugate(orientation)));

				position.x += (joint.pos.x + weightPos.x) * weight.bias;
				position.y += (joint.pos.y + weightPos.y) * weight.bias;
				position.z += (joint.pos.z + weightPos.z) * weight.bias;
			}
			subset.positions.push_back(position);
			positions.push_back(position);
		}
		//Calculate bounding sphere for the model subset based on the default pose.
		CreateBoundingSphere(subset.positions, subset.boundingSphere);
		totalVertices += subset.positions.size();
	}
	//Calculate bounding sphere for the model based on the default pose.
	CreateBoundingSphere(positions, model.boundingSphere);
}

void GeometryUtils::CreateMeshPolygon(const std::vector<DVertex>& vertices, int index1, int index2, int index3, MeshPolygon& meshPolygon)
{
	//Compute normal of MeshPolygon.
	XMVECTOR v1 = XMLoadFloat3(&vertices[index1].pos);
	XMVECTOR v2 = XMLoadFloat3(&vertices[index2].pos);
	XMVECTOR v3 = XMLoadFloat3(&vertices[index3].pos);

	XMVECTOR U = XMVectorSubtract(v2, v1);
	XMVECTOR V = XMVectorSubtract(v3, v1);

	//Set MeshPolygon normal to cross product of U and V.
	meshPolygon.normal = VF3(XMVector3Normalize(XMVector3Cross(U, V)));

	//Fill in MeshPolygon index information.
	meshPolygon.index1 = index1;
	meshPolygon.index2 = index2;
	meshPolygon.index3 = index3;
};

//Creates the smallest bounding sphere where the centre is the average of the points passed in.
void GeometryUtils::CreateBoundingSphere(std::vector<XMFLOAT3> const& points, BoundingSphere& sphere)
{
	std::vector<XMVECTOR> xmPoints;

	XMVECTOR centre = XMVectorSet(0, 0, 0, 0);

	for (int i = 0; i < points.size(); i++)
	{
		xmPoints.push_back(FV(points[i]));
		centre += xmPoints[i];
	}

	centre /= points.size();

	sphere.centre = VF3(centre);

	for (int i = 0; i < points.size(); i++)
	{
		float distSqr = XMVectorGetX(XMVector3LengthSq(xmPoints[i] - centre));
		if (distSqr > sphere.radius)
		{
			sphere.radius = distSqr;
		}
	}

	sphere.radius = sqrt(sphere.radius);
}

//Merges two spheres together.
void GeometryUtils::MergeBoundingSpheres(BoundingSphere const& sphere1, BoundingSphere const& sphere2, BoundingSphere& merge)
{
	XMVECTOR centre1 = FV(sphere1.centre);
	XMVECTOR centre2 = FV(sphere2.centre);
	XMVECTOR cenDiff = centre2 - centre1;
	float distSqr = XMVectorGetX(XMVector3LengthSq(cenDiff));
	float rDiff = sphere2.radius - sphere1.radius;
	float rDiffSqr = rDiff*rDiff;

	//Check if one sphere contains the other.
	if (rDiffSqr >= distSqr)
	{
		//If so, the merged sphere is the bigger one.
		merge = (rDiff >= 0.0f ? sphere2 : sphere1);
	}
	else
	{
		float dist = sqrt(distSqr);
		if (dist > 0.0f)
		{
			//The bigger the coefficient is, the bigger the second sphere is as opposed to the first sphere
			//and the centre of the mergeed sphere will be closer to the centre of the second sphere as a result.
			float coeff = (dist + rDiff) / (2 * dist);
			merge.centre = VF3(centre1 + coeff * cenDiff);
		}
		else
		{
			merge.centre = sphere1.centre;
		}

		merge.radius = 0.5f * (dist + sphere1.radius + sphere2.radius);
	}
}

void GeometryUtils::MergeBoundingSpheres(std::vector<BoundingSphere> const& spheres, BoundingSphere& merge)
{
	if (spheres.size() < 1)
	{
		return;
	}

	merge = spheres[0];

	for (uint32_t i = 1; i < spheres.size(); ++i)
	{
		MergeBoundingSpheres(merge, spheres[i], merge);
	}
}

Ray GeometryUtils::ComputeIntersectionLine(FXMVECTOR p1, FXMVECTOR p2)
{
	XMVECTOR direction = XMVector3Cross(p1, p2);
	XMVECTOR lengthSquared = XMVector3LengthSq(direction);
	XMVECTOR position = XMVector3Cross((-XMVectorGetW(p1) * p2) + (XMVectorGetW(p2) * p1), direction) / lengthSquared;

	return Ray(VF3(position), VF3(direction));
}

XMVECTOR GeometryUtils::ComputeIntersection(FXMVECTOR& plane, const Ray& ray)
{
	float value = (-XMVectorGetW(plane) - XMVectorGetX(XMVector3Dot(plane, FV(ray.position)))) / XMVectorGetX(XMVector3Dot(plane, FV(ray.direction)));

	return (FV(ray.position) + (FV(ray.direction) * value));
}

