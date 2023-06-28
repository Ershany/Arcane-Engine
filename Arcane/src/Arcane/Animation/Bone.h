#pragma once
#ifndef BONE_H
#define BONE_H

struct aiNodeAnim;

namespace Arcane
{
	struct KeyPosition
	{
		glm::vec3 position;
		float timestamp;
	};

	struct KeyRotation
	{
		glm::quat orientation;
		float timestamp;
	};

	struct KeyScale
	{
		glm::vec3 scale;
		float timestamp;
	};

	class Bone
	{
	public:
		Bone(const std::string& name, int id, const aiNodeAnim* channel);

		void Update(float currentAnimationTime);
		int GetPositionIndex(float currentAnimationTime);
		int GetRotationIndex(float currentAnimationTime);
		int GetScaleIndex(float currentAnimationTime);

		inline const std::string& GetName() const { return m_Name; }
		inline const glm::mat4& GetLocalTransform() const { return m_LocalTransform; }
	private:
		float GetNormalizedInterpolationAmountBetweenFrames(float lastTimestamp, float nextTimestamp, float currentAnimationTime);
		glm::mat4 InterpolatePosition(float currentAnimationTime);
		glm::mat4 InterpolateRotation(float currentAnimationTime);
		glm::mat4 InterpolateScale(float currentAnimationTime);
	private:
		std::vector<KeyPosition> m_Positions;
		std::vector<KeyRotation> m_Rotations;
		std::vector<KeyScale> m_Scales;

		glm::mat4 m_LocalTransform;
		std::string m_Name;
		int m_ID;
	};
}
#endif
