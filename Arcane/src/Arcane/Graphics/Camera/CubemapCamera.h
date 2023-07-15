#pragma once
#ifndef CUBEMAPCAMERA_H
#define CUBEMAPCAMERA_H

#ifndef ICAMERA_H
#include <Arcane/Graphics/Camera/ICamera.h>
#endif

namespace Arcane
{
	class CubemapCamera : public ICamera {
	public:
		CubemapCamera();
		CubemapCamera(glm::vec3 &centerPosition, float nearPlane = DEFAULT_NEAR_PLANE, float farPlane = DEFAULT_FAR_PLANE);

		void SwitchCameraToFace(int cubeFaceIndex);

		virtual glm::mat4 GetViewMatrix() override;
		virtual glm::mat4 GetProjectionMatrix() override;
		inline virtual const glm::vec3& GetFront() const override { return m_Front; }
		inline virtual const glm::vec3& GetUp() const override { return m_Up; }
		inline virtual void InvertPitch() override {}
	private:
		glm::vec3 m_Front, m_Up;
	};
}
#endif
