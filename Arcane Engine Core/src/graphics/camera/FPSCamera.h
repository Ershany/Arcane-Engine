#pragma once

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
	const GLfloat FOV = 45.0f;

	class FPSCamera {
	public:
		// Camera Attributes
		glm::vec3 m_Position, m_Front, m_Up, m_Right, m_WorldUp;
		// Euler Angles
		GLfloat m_Yaw;
		GLfloat m_Pitch;
		// Camera Options
		GLfloat m_MovementSpeed;
		GLfloat m_MouseSensitivity;
		GLfloat m_FOV;

		// Vector Constuctor
		FPSCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH);
		// Scalar Constructor
		FPSCamera(GLfloat xPos, GLfloat yPos, GLfloat zPos, GLfloat xUp, GLfloat yUp, GLfloat zUp, GLfloat yaw, GLfloat pitch);
		
		glm::mat4 getViewMatrix();
		void processKeyboard(Camera_Movement direction, GLfloat deltaTime);
		void processMouseMovement(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch = true);
		void processMouseScroll(GLfloat yOffset);
	private:
		void updateCameraVectors();
	};

} }