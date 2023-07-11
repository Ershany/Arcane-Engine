#pragma once
#ifndef ICAMERA_H
#define ICAMERA_H

namespace Arcane
{
	class ICamera {
	public:
		ICamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float nearPlane = DEFAULT_NEAR_PLANE, float farPlane = DEFAULT_FAR_PLANE) : m_Position(position), m_NearPlane(nearPlane), m_FarPlane(farPlane) {}
		virtual ~ICamera() {}

		virtual glm::mat4 GetViewMatrix() = 0;
		virtual glm::mat4 GetProjectionMatrix() = 0;

		virtual const glm::vec3& GetPosition() const { return m_Position; }
		virtual const float GetNearPlane() const { return m_NearPlane; }
		virtual const float GetFarPlane() const { return m_FarPlane; }
		virtual const glm::vec3& GetFront() const = 0;
		virtual const glm::vec3& GetUp() const = 0;

		virtual void SetPosition(const glm::vec3 &position) { m_Position = position; }
		virtual void SetNearFarPlane(float nearPlane, float farPlane) { m_NearPlane = nearPlane; m_FarPlane = farPlane; }
		virtual void InvertPitch() = 0;
	protected:
		glm::vec3 m_Position;
		float m_NearPlane, m_FarPlane;
	};
}
#endif
