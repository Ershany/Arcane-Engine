#pragma once

#include <Arcane/Animation/Bone.h>
#include <Arcane/Animation/AnimationData.h>
#include <Arcane/Graphics/Mesh/Model.h>

struct aiAnimation;
struct aiNode;

namespace Arcane
{
	class Model;

	struct AssimpNodeData
	{
		glm::mat4 transformation;
		std::string name;
		int childCount;
		std::vector<AssimpNodeData> children;
	};

	class AnimationClip
	{
	public:
		AnimationClip(const std::string &animationPath, Model *model);
		~AnimationClip();

		Bone* FindBone(const std::string &name);

		inline float GetDuration() { return m_ClipDuration; }
		inline float GetTicksPerSecond() { return m_TicksPerSecond; }
		inline const AssimpNodeData* GetRootNode() const { return &m_RootNode; }
		inline auto* GetBoneDataMap() { return m_Model->GetBoneDataMap(); }
	private:
		void ReadMissingBones(const aiAnimation *animation, Model *model);
		void ReadHierarchyData(AssimpNodeData &dest, const aiNode *src);
	private:
		float m_ClipDuration;
		float m_TicksPerSecond;
		std::vector<Bone> m_Bones;
		AssimpNodeData m_RootNode;
		Model *m_Model;
	};
}
