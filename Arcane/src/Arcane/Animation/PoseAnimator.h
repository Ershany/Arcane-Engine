#pragma once

namespace Arcane
{
	class AnimationClip;
	struct AssimpBoneData;

	class PoseAnimator
	{
	public:
		PoseAnimator();

		void UpdateAnimation(float deltaTime);
		void SetAnimationClip(AnimationClip *clip);

		inline AnimationClip* GetCurrentAnimationClip() { return m_CurrentAnimationClip; }
		inline const std::vector<glm::mat4>& GetFinalBoneMatrices() const { return m_FinalBoneMatrices; }
	private:
		void CalculateBoneTransform(const AssimpBoneData *node, glm::mat4 parentTransform);
	private:
		std::vector<glm::mat4> m_FinalBoneMatrices;
		AnimationClip *m_CurrentAnimationClip;
		float m_CurrentTime;

		bool m_PlayClipIndefinitely = true;
	};
}
