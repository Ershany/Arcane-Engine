#include "arcpch.h"
#include "AnimationClip.h"

#include <assimp/Importer.hpp>

namespace Arcane
{
	AnimationClip::AnimationClip(const std::string &animationPath, int animationIndex, Model *model) : m_Model(model)
	{
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
		ARC_ASSERT(scene && scene->mRootNode, "Failed importing animationPath");

		m_AssimpAnimation = scene->mAnimations[animationIndex];
		m_ClipDuration = static_cast<float>(m_AssimpAnimation->mDuration);
		m_TicksPerSecond = m_AssimpAnimation->mTicksPerSecond != 0 ? static_cast<float>(m_AssimpAnimation->mTicksPerSecond) : 1.0f;

		// Setup our root node bone and recursively build the others
		ReadHierarchyData(m_RootNode, scene->mRootNode);
		ReadMissingBones();
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

		// If we didn't find just return nullptr
		if (iter == m_Bones.end()) return nullptr;
		
		// Otherwise we found it
		return &(*iter);
	}

	void AnimationClip::ReadMissingBones()
	{
		int size = m_AssimpAnimation->mNumChannels;

		auto boneInfoMap = m_Model->GetBoneDataMap();
		int &boneCount = m_Model->GetBoneCountRef();

		// Sometimes we miss bones, so this function will find any other bones engaged in the animation and add them. Assimp struggles..
		for (int i = 0; i < size; i++)
		{
			auto channel = m_AssimpAnimation->mChannels[i];
			std::string boneName = channel->mNodeName.data;

			if (boneInfoMap->find(boneName) == boneInfoMap->end())
			{
				(*boneInfoMap)[boneName].boneID = boneCount++;
			}
			m_Bones.push_back(Bone(channel->mNodeName.data, (*boneInfoMap)[channel->mNodeName.data].boneID, channel));
		}
	}

	void AnimationClip::ReadHierarchyData(AssimpBoneData &dest, const aiNode *src)
	{
		ARC_ASSERT(src, "Needs src data to read in AnimationClip");

		dest.name = src->mName.data;
		dest.transformation = Model::ConvertAssimpMatrixToGLM(src->mTransformation);
		dest.childCount = src->mNumChildren;
		dest.children.reserve(dest.childCount);

		for (unsigned int i = 0; i < src->mNumChildren; i++)
		{
			AssimpBoneData newData;
			ReadHierarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}
}
