#pragma once

namespace Arcane
{
	class AnimationClip;
	struct AssimpNodeData;

	class PoseAnimator
	{
	public:
		PoseAnimator();

		void UpdateAnimation(float deltaTime);
		void PlayAnimation(AnimationClip *clip);

		void CalculateBoneTransform(const AssimpNodeData *node, glm::mat4 parentTransform);
	private:
		std::vector<glm::mat4> m_FinalBoneMatrices;
		AnimationClip *m_CurrentAnimationClip;
		float m_CurrentTime;
		float m_DeltaTime;
	};
}
