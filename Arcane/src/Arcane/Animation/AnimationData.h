#pragma once

namespace Arcane
{
	constexpr int MaxBonesPerModel = 100; // Also will need to be adjusted in the appropriate skinned shaders
	constexpr int MaxBonesPerVertex = 4; // Also will need to be adjusted in the appropriate skinned shaders

	struct VertexBoneData
	{
		int BoneIDs[MaxBonesPerVertex];
		float Weights[MaxBonesPerVertex];
	};

	struct BoneData
	{
		int boneID;
		glm::mat4 inverseBindPose; // We upload the vertices already in bind pose. Since we are transforming the bones we need to undo this transformation by using the inverse bind pose on the GPU before we can apply rotations and stuff to each vertex
	};
}
