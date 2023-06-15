#include "arcpch.h"
#include "PoseAnimator.h"

#include <Arcane/Animation/AnimationClip.h>
#include <Arcane/Animation/AnimationData.h>

namespace Arcane
{
	PoseAnimator::PoseAnimator() 
		: m_CurrentAnimationClip(nullptr), m_CurrentTime(0.0f), m_FinalBoneMatrices(100, glm::mat4(1.0f))
	{}

	void PoseAnimator::UpdateAnimation(float deltaTime)
	{
		// Play animation clip
		if (m_CurrentAnimationClip)
		{
			m_CurrentTime += m_CurrentAnimationClip->GetTicksPerSecond() * deltaTime;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimationClip->GetDuration());
			CalculateBoneTransform(m_CurrentAnimationClip->GetRootBone(), glm::mat4(1.0f));
		}
	}

	void PoseAnimator::SetAnimationClip(AnimationClip *clip)
	{
		m_CurrentAnimationClip = clip;
		m_CurrentTime = 0.0f;
	}

	void PoseAnimator::CalculateBoneTransform(const AssimpBoneData *node, glm::mat4 parentTransform)
	{
		const std::string &nodeName = node->name;
		glm::mat4 currentTransform;

		// Get the current bone engaged in the animation
		Bone *bone = m_CurrentAnimationClip->FindBone(nodeName);
		if (bone)
		{
			// Update the bones local transform with the keyframe data when we supply it the current animation time
			bone->Update(m_CurrentTime);
			currentTransform = bone->GetLocalTransform();
		}
		else
		{
			// If we can't find the bone, then just default to the node's transformation
			currentTransform = node->transformation;
		}

		// Calculate the total transformation given its parent
		glm::mat4 globalTransformation = parentTransform * currentTransform;

		// We need to apply the inverse bind pose to our globalTransformation. This is necessary because the model starts in bind pose
		// and you need to animate a vertex, you need to transform it to the bone's local coordinate system, calculate the transformation and move it back into world space in the shader
		auto boneDataMap = m_CurrentAnimationClip->GetBoneDataMap();
		if (boneDataMap->find(nodeName) != boneDataMap->end())
		{
			int index = (*boneDataMap)[nodeName].boneID;
			ARC_ASSERT(index < MaxBonesPerModel, "We exceeded the MaxBonesPerModel limit");
			glm::mat4 inverseBindPose = (*boneDataMap)[nodeName].inverseBindPose;
			m_FinalBoneMatrices[index] = globalTransformation * inverseBindPose;
		}

		for (int i = 0; i < node->childCount; i++)
		{
			CalculateBoneTransform(&node->children[i], globalTransformation);
		}
	}
}
