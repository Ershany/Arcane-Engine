#pragma once

namespace Arcane
{
	class ICamera {
	public:
		virtual ~ICamera() {}

		virtual glm::mat4 GetViewMatrix() = 0;
		virtual glm::mat4 GetProjectionMatrix(float nearPlane = NEAR_PLANE, float farPlane = FAR_PLANE) = 0;

		virtual const glm::vec3& GetPosition() const = 0;
		virtual const glm::vec3& GetFront() const = 0;
		virtual const glm::vec3& GetUp() const = 0;

		virtual void SetPosition(const glm::vec3 &position) = 0;
		virtual void InvertPitch() = 0;
	};
}
