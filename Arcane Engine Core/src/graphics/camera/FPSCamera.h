#pragma once

#include <graphics/Window.h>
#include <ui/DebugPane.h>

namespace arcane {

	enum Camera_Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UPWARDS,
		DOWNWARDS
	};

	// Default Camera Values
	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 40.0f;
	const float SENSITIVITY = 0.10f;
	const float FOV = 100.0f;

	class FPSCamera {
	private:
		// Camera Attributes
		glm::vec3 m_Position, m_Front, m_Up, m_Right, m_WorldUp;

		// Euler Angles
		float m_Yaw;
		float m_Pitch;

		// Camera Options
		float m_MovementSpeed;
		float m_MouseSensitivity;
		float m_FOV;
	public:
		/**
		* Constructs a camera using the position and up vectors
		* Default camera front is in vec3(0, 0, -1) direction
		*
		* @param position Optional. The initial position of the camera. Default is 0
		* @param up Optional. Represents the up position of the camera. Default is 0
		* @param yaw Optional. Initial yaw of the camera. Default is determined by the Camera class
		* @param pitch Optional. Initial pitch of the camera. Default is determined by the Camera class
		*/
		FPSCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

		/**
		* Constructs a camera using floats to represent the position and up vectors of the camera
		* Default camera front is in vec3(0, 0, -1) direction
		*
		* @param xPos Starting x position of the camera
		* @param yPos Starting y position of the camera
		* @param zPos Starting z position of the camera
		* @param xUp X-Coordinate that represents the camera's up position
		* @param yUp Y-Coordinate that represents the camera's up position
		* @param zUp Z-Coordinate that represents the camera's up position
		* @param yaw Optional. Initial yaw of the camera. Default is determined by the Camera class
		* @param pitch Optional. Initial pitch of the camera. Default is determined by the Camera class
		*/
		FPSCamera(float xPos, float yPos, float zPos, float xUp, float yUp, float zUp, float yaw, float pitch);


		glm::mat4 getViewMatrix();
		glm::mat4 getProjectionMatrix();

		void processInput(float deltaTime);

		// Getters
		inline float getYaw() const { return m_Yaw; }
		inline float getPitch() const { return m_Pitch; }
		inline float getMovementSpeed() const { return m_MovementSpeed; }
		inline float getMouseSensitivity() const { return m_MouseSensitivity; }
		inline float getFOV() const { return m_FOV; }
		inline const glm::vec3& getFront() const { return m_Front; }
		inline const glm::vec3& getPosition() const { return m_Position; }
	private:
		/**
		* Updates the front, right, and up vectors for the camera
		* These vectors are reliant on the yaw and pitch of the camera
		*/
		void updateCameraVectors();

		/**
		* Translates the position of the camera
		*
		* @param direction Enumeration type for the direction to move the camera
		* @param deltaTime Delta time since the last update
		*/
		void processKeyboard(Camera_Movement direction, float deltaTime);

		/**
		* Changes what the camera is looking at, by adding the offsets with a sensitivity setting
		* May also constrain the pitch
		*
		* @param xOffset The value that is added to the x-offset
		* @param yOffset The value that is added to the y-offset
		* @param constrainPitch Optional. Option to constrain the pitch. Default is true
		*/
		void processMouseMovement(double xOffset, double yOffset, GLboolean constrainPitch);

		/**
		* Changes the camera's Field of View
		* Ensures that it doesn't go below 1 or above the maximum Field of View
		*
		* @param offset The value that is added to the Field of View
		*/
		void processMouseScroll(double yOffset);
	};

}
