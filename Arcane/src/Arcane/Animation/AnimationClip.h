#pragma once
#ifndef ANIMATIONCLIP_H
#define ANIMATIONCLIP_H

#ifndef BONE_H
#include <Arcane/Animation/Bone.h>
#endif

#ifndef MODEL_H
#include <Arcane/Graphics/Mesh/Model.h>
#endif

#ifndef AI_ANIM_H_INC
#include "assimp/anim.h"
#endif

struct aiNode;

namespace Arcane
{
	class Model;

	struct AssimpBoneData
	{
		glm::mat4 transformation;
		std::string name;
		int childCount;
		std::vector<AssimpBoneData> children;
	};

	class AnimationClip
	{
	public:
		AnimationClip(const std::string &animationPath, int animationIndex, Model *model);
		~AnimationClip();

		Bone* FindBone(const std::string &name);

		inline float GetDuration() { return m_ClipDuration; }
		inline float GetTicksPerSecond() { return m_TicksPerSecond; }
		inline const AssimpBoneData* GetRootBone() const { return &m_RootNode; }
		inline auto* GetBoneDataMap() { return m_Model->GetBoneDataMap(); }
		inline const auto& GetGlobalInverseTransform() const { return m_Model->GetGlobalInverseTransform(); }
#if !ARC_FINAL
		std::string GetAnimationName() { return m_AnimationName; }
#endif
	private:
		void ReadMissingBones(aiAnimation *assimpAnimation);
		void ReadHierarchyData(AssimpBoneData &dest, const aiNode *src);
	private:
		float m_ClipDuration;
		float m_TicksPerSecond;
		std::vector<Bone> m_Bones;
		AssimpBoneData m_RootNode;

#if !ARC_FINAL
		std::string m_AnimationName;
#endif
		Model *m_Model;
	};
}
#endif
