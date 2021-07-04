#pragma once

#include <graphics/camera/ICamera.h>

namespace Arcane
{
	class CubemapCamera : public ICamera {
	public:
		CubemapCamera();
		CubemapCamera(glm::vec3 &centerPosition);

		void SwitchCameraToFace(int cubeFaceIndex);
		inline void SetCenterPosition(glm::vec3 &newPosition) { m_CenterPosition = newPosition; }

		virtual glm::mat4 GetViewMatrix() override;
		virtual glm::mat4 GetProjectionMatrix() override;
		inline virtual const glm::vec3& GetPosition() const override { return m_CenterPosition; }
		inline virtual const glm::vec3& GetFront() const override { return m_Front; }
		inline virtual const glm::vec3& GetUp() const override { return m_Up; }
		inline virtual void SetPosition(const glm::vec3 &position) override { m_CenterPosition = position; };
		inline virtual void InvertPitch() override {}
	private:
		glm::vec3 m_CenterPosition, m_Front, m_Up;
	};
}
