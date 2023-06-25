#pragma once

#include <Arcane/Animation/Bone.h>
#include <Arcane/Animation/AnimationData.h>
#include <Arcane/Graphics/Mesh/Model.h>

struct aiAnimation;
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
		inline const char* GetAnimationName() const { return m_AssimpAnimation->mName.C_Str(); }
	private:
		void ReadMissingBones();
		void ReadHierarchyData(AssimpBoneData &dest, const aiNode *src);
	private:
		float m_ClipDuration;
		float m_TicksPerSecond;
		std::vector<Bone> m_Bones;
		AssimpBoneData m_RootNode;

		Model *m_Model;
		aiAnimation *m_AssimpAnimation;
	};
}
