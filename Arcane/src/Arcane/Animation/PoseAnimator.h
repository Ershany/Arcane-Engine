#pragma once

namespace Arcane
{
	class AnimationClip;
	struct AssimpBoneData;

	class PoseAnimator
	{
	public:
		PoseAnimator();

		void PlayAnimation(float deltaTime);
		void SetAnimationClip(AnimationClip *clip);
	private:
		void CalculateBoneTransform(const AssimpBoneData *node, glm::mat4 parentTransform);
	private:
		std::vector<glm::mat4> m_FinalBoneMatrices;
		AnimationClip *m_CurrentAnimationClip;
		float m_CurrentTime;
		float m_DeltaTime;
	};
}
