#include "arcpch.h"
#include "AnimationClip.h"

#include <assimp/Importer.hpp>

namespace Arcane
{
	AnimationClip::AnimationClip(const std::string &animationPath, Model *model)
	{
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
		ARC_ASSERT(scene && scene->mRootNode, "Failed importing animationPath");

		auto animation = scene->mAnimations[0];
		m_ClipDuration = static_cast<float>(animation->mDuration);
		m_TicksPerSecond = static_cast<float>(animation->mTicksPerSecond);
		ReadHierarchyData(m_RootNode, scene->mRootNode);
		ReadMissingBones(animation, model);
	}

	AnimationClip::~AnimationClip()
	{

	}

	Bone* AnimationClip::FindBone(const std::string &name)
	{
		auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
			[&](const Bone& bone)
			{
				return bone.GetName() == name;
			}
		);
		return nullptr;
	}

	void AnimationClip::ReadMissingBones(const aiAnimation *animation, Model *model)
	{
		int size = animation->mNumChannels;

		auto boneInfoMap = model->GetBoneDataMap();
		int &boneCount = model->GetBoneCountRef();

		// reading channels(bones engaged in an animation and their keyframes)
		for (int i = 0; i < size; i++)
		{
			auto channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;

			if (boneInfoMap->find(boneName) == boneInfoMap->end())
			{
				(*boneInfoMap)[boneName].boneID = boneCount++;
			}
			m_Bones.push_back(Bone(channel->mNodeName.data, (*boneInfoMap)[channel->mNodeName.data].boneID, channel));
		}
	}

	void AnimationClip::ReadHierarchyData(AssimpNodeData &dest, const aiNode *src)
	{
		ARC_ASSERT(src, "Needs src data to read in AnimationClip");

		dest.name = src->mName.data;
		dest.transformation = Model::ConvertAssimpMatrixToGLM(src->mTransformation);
		dest.childCount = src->mNumChildren;

		for (unsigned int i = 0; i < src->mNumChildren; i++)
		{
			AssimpNodeData newData;
			ReadHierarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}
}
