#include "arcpch.h"
#include "Bone.h"

#include <assimp/anim.h>

namespace Arcane
{
	Bone::Bone(const std::string &name, int id, const aiNodeAnim *channel) : m_Name(name), m_ID(id), m_LocalTransform(1.0f)
	{
		m_Positions.reserve(channel->mNumPositionKeys);
		m_Rotations.reserve(channel->mNumRotationKeys);
		m_Scales.reserve(channel->mNumScalingKeys);

		for (int i = 0; i < m_Positions.capacity(); i++)
		{
			const aiVector3D &aiPosition = channel->mPositionKeys[i].mValue;

			KeyPosition data;
			data.position = glm::vec3(aiPosition.x, aiPosition.y, aiPosition.z);
			data.timestamp = static_cast<float>(channel->mPositionKeys[i].mTime);
			m_Positions.push_back(std::move(data));
		}
		for (int i = 0; i < m_Rotations.capacity(); i++)
		{
			const aiQuaternion &aiOrientation = channel->mRotationKeys[i].mValue;

			KeyRotation data;
			data.orientation = glm::quat(aiOrientation.w, aiOrientation.x, aiOrientation.y, aiOrientation.z);
			data.timestamp = static_cast<float>(channel->mRotationKeys[i].mTime);
			m_Rotations.push_back(std::move(data));
		}
		for (int i = 0; i < m_Scales.capacity(); i++)
		{
			const aiVector3D &scale = channel->mScalingKeys[i].mValue;

			KeyScale data;
			data.scale = glm::vec3(scale.x, scale.y, scale.z);
			data.timestamp = static_cast<float>(channel->mScalingKeys[i].mTime);
			m_Scales.push_back(std::move(data));
		}
	}

	// Interpolates between positions, rotations, and scaling keys based on the current timestep were at in the animation and prepares the local transform for the bone given this info
	void Bone::Update(float currentAnimationTime)
	{
		glm::mat4 translation = InterpolatePosition(currentAnimationTime);
		glm::mat4 rotation = InterpolateRotation(currentAnimationTime);
		glm::mat4 scale = InterpolateScale(currentAnimationTime);
		m_LocalTransform = translation * rotation * scale;
	}

	float Bone::GetNormalizedInterpolationAmountBetweenFrames(float lastTimestamp, float nextTimestamp, float currentAnimationTime)
	{
		float widwayLength = currentAnimationTime - lastTimestamp;
		float framesDiff = nextTimestamp - lastTimestamp;
		return widwayLength / framesDiff;
	}

	glm::mat4 Bone::InterpolatePosition(float currentAnimationTime)
	{
		if (m_Positions.size() == 1)
			return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

		int index0 = GetPositionIndex(currentAnimationTime);
		int index1 = index0 + 1;
		float lerpValue = GetNormalizedInterpolationAmountBetweenFrames(m_Positions[index0].timestamp, m_Positions[index1].timestamp, currentAnimationTime);

		// Finally LERP between our position data for our animation frames
		glm::vec3 finalPosition = glm::mix(m_Positions[index0].position, m_Positions[index1].position, lerpValue);
		return glm::translate(glm::mat4(1.0f), finalPosition);
	}

	glm::mat4 Bone::InterpolateRotation(float currentAnimationTime)
	{
		if (m_Rotations.size() == 1)
		{
			auto rotation = glm::normalize(m_Rotations[0].orientation);
			return glm::toMat4(rotation);
		}

		int index0 = GetRotationIndex(currentAnimationTime);
		int index1 = index0 + 1;
		float slerpValue = GetNormalizedInterpolationAmountBetweenFrames(m_Rotations[index0].timestamp, m_Rotations[index1].timestamp, currentAnimationTime);

		// Finally SLERP between our rotation data for our animation frames
		glm::quat finalRotation = glm::slerp(m_Rotations[index0].orientation, m_Rotations[index1].orientation, slerpValue);
		finalRotation = glm::normalize(finalRotation);
		return glm::toMat4(finalRotation);
	}

	glm::mat4 Bone::InterpolateScale(float currentAnimationTime)
	{
		if (m_Scales.size() == 1)
			return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

		int index0 = GetScaleIndex(currentAnimationTime);
		int index1 = index0 + 1;
		float lerpValue = GetNormalizedInterpolationAmountBetweenFrames(m_Scales[index0].timestamp, m_Scales[index1].timestamp, currentAnimationTime);

		// Finally LERP between our scale data for our animation frames
		glm::vec3 finalScale = glm::mix(m_Scales[index0].scale, m_Scales[index1].scale, lerpValue);
		return glm::scale(glm::mat4(1.0f), finalScale);
	}

	// TODO: At least binary search this..
	int Bone::GetPositionIndex(float currentAnimationTime)
	{
		for (int i = 0; i < m_Positions.size() - 1; i++)
		{
			if (currentAnimationTime < m_Positions[i + 1].timestamp)
			{
				return i;
			}
		}

		ARC_ASSERT(false, "No position keyframe was found, something is wrong");
		return 0;
	}

	// TODO: At least binary search this..
	int Bone::GetRotationIndex(float currentAnimationTime)
	{
		for (int i = 0; i < m_Rotations.size() - 1; i++)
		{
			if (currentAnimationTime < m_Rotations[i + 1].timestamp)
			{
				return i;
			}
		}

		ARC_ASSERT(false, "No rotation keyframe was found, something is wrong");
		return 0;
	}

	// TODO: At least binary search this..
	int Bone::GetScaleIndex(float currentAnimationTime)
	{
		for (int i = 0; i < m_Scales.size() - 1; i++)
		{
			if (currentAnimationTime < m_Scales[i + 1].timestamp)
			{
				return i;
			}
		}

		ARC_ASSERT(false, "No scale keyframe was found, something is wrong");
		return 0;
	}

}
