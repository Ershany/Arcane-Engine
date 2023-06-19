#pragma once

namespace Arcane
{
	constexpr int MaxBonesPerModel = 100; // Also will need to be adjusted in the appropriate skinned shaders
	constexpr int MaxBonesPerVertex = 4; // Also will need to be adjusted in the appropriate skinned shaders

	// Data structure used for loading bone data from assimp and storing into our vertex buffers
	struct VertexBoneData
	{
		int BoneIDs[MaxBonesPerVertex];
		float Weights[MaxBonesPerVertex];
	};

	struct BoneData
	{
		int boneID;

		/*
		Matrix that transforms from bone space to mesh space in bind pose.
		This matrix describes the position of the mesh in the local space of this bone when the skeleton was bound.
		Thus it can be used directly to determine a desired vertex position, given the world-space transform of the bone when animated, and the position of the vertex in mesh space.
		*/
		glm::mat4 inverseBindPose;
	};
}
