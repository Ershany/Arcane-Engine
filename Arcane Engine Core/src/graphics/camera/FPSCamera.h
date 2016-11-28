#pragma once

#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace arcane { namespace graphics {

	enum Camera_Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	// Default Camera Values
	const GLfloat YAW = -90.0f;
	const GLfloat PITCH = 0.0f;
	const GLfloat SPEED = 3.0f;
	const GLfloat SENSITIVITY = 0.25f;
	const GLfloat FOV = 80.0f;

	class FPSCamera {
	private:
		// Camera Attributes
		glm::vec3 m_Position, m_Front, m_Up, m_Right, m_WorldUp;
		// Euler Angles
		GLfloat m_Yaw;
		GLfloat m_Pitch;
		// Camera Options
		GLfloat m_MovementSpeed;
		GLfloat m_MouseSensitivity;
		GLfloat m_FOV;
	public:
		// Vector Constuctor
		FPSCamera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch);
		// Scalar Constructor
		FPSCamera(GLfloat xPos, GLfloat yPos, GLfloat zPos, GLfloat xUp, GLfloat yUp, GLfloat zUp, GLfloat yaw, GLfloat pitch);
		
		glm::mat4 getViewMatrix();
		void processKeyboard(Camera_Movement direction, GLfloat deltaTime);
		void processMouseMovement(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch);
		void processMouseScroll(GLfloat yOffset);

		inline GLfloat getYaw() { return m_Yaw; }
		inline GLfloat getPitch() { return m_Pitch; }
		inline GLfloat getMovementSpeed() { return m_MovementSpeed; }
		inline GLfloat getMouseSensitivity() { return m_MouseSensitivity; }
		inline GLfloat getFOV() { return m_FOV; }
	private:
		void updateCameraVectors();
	};

} }