#include "arcpch.h"
#include "PoseAnimator.h"

#include <Arcane/Animation/AnimationClip.h>
#include <Arcane/Animation/AnimationData.h>

namespace Arcane
{
	PoseAnimator::PoseAnimator() 
		: m_CurrentAnimationClip(nullptr), m_CurrentTime(0.0f), m_DeltaTime(0.0f), m_FinalBoneMatrices(100, glm::mat4(1.0f))
	{}

	void PoseAnimator::UpdateAnimation(float deltaTime)
	{
		m_DeltaTime = deltaTime;
		if (m_CurrentAnimationClip)
		{
			m_CurrentTime += m_CurrentAnimationClip->GetTicksPerSecond() * deltaTime;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimationClip->GetDuration());
			CalculateBoneTransform(m_CurrentAnimationClip->GetRootNode(), glm::mat4(1.0f));
		}
	}

	void PoseAnimator::PlayAnimation(AnimationClip *clip)
	{
		m_CurrentAnimationClip = clip;
		m_CurrentTime = 0.0f;
	}

	void PoseAnimator::CalculateBoneTransform(const AssimpNodeData *node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 currentTransform;

		Bone *bone = m_CurrentAnimationClip->FindBone(nodeName);
		if (bone)
		{
			bone->Update(m_CurrentTime);
			currentTransform = bone->GetLocalTransform();
		}
		else
		{
			currentTransform = node->transformation;
		}

		glm::mat4 globalTransformation = parentTransform * currentTransform;

		auto boneDataMap = m_CurrentAnimationClip->GetBoneDataMap();
		if (boneDataMap->find(nodeName) != boneDataMap->end())
		{
			int index = (*boneDataMap)[nodeName].boneID;
			glm::mat4 inverseBindPose = (*boneDataMap)[nodeName].inverseBindPose;
			m_FinalBoneMatrices[index] = globalTransformation * inverseBindPose;
		}

		for (int i = 0; i < node->childCount; i++)
		{
			CalculateBoneTransform(&node->children[i], globalTransformation);
		}
	}
}
