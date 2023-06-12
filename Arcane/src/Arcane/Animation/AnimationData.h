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

	// Stores bone index (this will be the same index used for final bone transformation matrix) and stores the inverse bind pose so we can apply it before doing any transformations
	struct BoneData
	{
		int boneID;
		glm::mat4 inverseBindPose; // We upload the vertices already in bind pose. Since we are transforming the bones we need to undo this transformation by using the inverse bind pose before we can apply rotations and stuff for animations
	};
}
