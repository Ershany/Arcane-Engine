#pragma once

#include "../Model.h"
#include <glm\glm.hpp>

namespace arcane { namespace graphics {

	class Renderable3D {
	public:
		Renderable3D(glm::vec3 &position, glm::vec3 &scale, glm::vec3 &rotationAxis, float radianRotation, Model *model, bool shouldOutline = false, bool transparent = false);
		~Renderable3D();

		void draw(Shader &shader) const;


		inline glm::vec3 getPosition() { return m_Position; }
		inline glm::vec3 getScale() { return m_Scale; }
		inline glm::vec3 getRotationAxis() { return m_RotationAxis; }
		inline float getRadianRotation() { return m_RadianRotation; }
		inline bool getShouldOutline() { return m_ShouldOutline; }
		inline bool getTransparent() { return m_Transparent; }

		inline void setPosition(glm::vec3 &other) { m_Position = other; }
		inline void setScale(glm::vec3 &other) { m_Scale = other; }
		inline void setRotationAxis(glm::vec3 &other) { m_RotationAxis = other; }
		inline void setRadianRotation(float value) { m_RadianRotation = value; }
		inline void setShouldOutline(bool choice) { m_ShouldOutline = choice; }
		inline void setTransparent(bool choice) { m_Transparent = choice; }
	private:
		glm::vec3 m_Position, m_Scale, m_RotationAxis;
		float m_RadianRotation;

		// Graphic features
		bool m_ShouldOutline, m_Transparent;
		
		Model *m_Model;
	};

} }