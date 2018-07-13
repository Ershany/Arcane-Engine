#pragma once

#include "../mesh/Model.h"
#include <glm\glm.hpp>

namespace arcane { namespace graphics {

	class Renderable3D {
	public:
		Renderable3D(glm::vec3 &position, glm::vec3 &scale, glm::vec3 &rotationAxis, float radianRotation, Model *model, Renderable3D *parent, bool shouldOutline = false, bool transparent = false);
		~Renderable3D();

		// Assumes shader is already bound by the renderer. This also allows the renderer to have total control
		// TODO: Add toggle in renderable that will skip the texture/material binding stage, which allows the renderer to have even more control
		void draw(Shader &shader) const;


		inline const glm::vec3& getPosition() const { return m_Position; }
		inline const glm::vec3& getScale() const { return m_Scale; }
		inline const glm::quat& getOrientation() const { return m_Orientation; }
		inline const Renderable3D* getParent() const { return m_Parent; }
		inline bool getShouldOutline() const { return m_ShouldOutline; }
		inline bool getTransparent() const { return m_Transparent; }

		inline void setPosition(glm::vec3 &other) { m_Position = other; }
		inline void setScale(glm::vec3 &other) { m_Scale = other; }
		inline void setShouldOutline(bool choice) { m_ShouldOutline = choice; }
		inline void setTransparent(bool choice) { m_Transparent = choice; }
	private:
		glm::vec3 m_Position, m_Scale;
		glm::quat m_Orientation;
		Renderable3D *m_Parent;

		// Graphic features
		bool m_ShouldOutline, m_Transparent;
		
		Model *m_Model;
	};

} }