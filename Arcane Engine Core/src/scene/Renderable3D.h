#pragma once

#include <graphics/mesh/Model.h>
#include <graphics/renderer/RenderPass.h>

namespace arcane {

	class Renderable3D {
	public:
		Renderable3D(glm::vec3 &position, glm::vec3 &scale, glm::vec3 &rotationAxis, float radianRotation, Model *model, Renderable3D *parent, bool transparent = false);
		~Renderable3D();

		// Assumes shader is already bound by the renderer
		void draw(Shader &shader, RenderPass pass) const;

		void addChild(Renderable3D *child);


		inline const glm::vec3& getPosition() const { return m_Position; }
		inline const glm::vec3& getScale() const { return m_Scale; }
		inline const glm::quat& getOrientation() const { return m_Orientation; }
		inline const Renderable3D* getParent() const { return m_Parent; }
		inline bool getTransparent() const { return m_Transparent; }

		inline void setPosition(glm::vec3 &other) { m_Position = other; }
		inline void setScale(glm::vec3 &other) { m_Scale = other; }
		inline void setOrientation(float radianRotation, glm::vec3 rotationAxis) { m_Orientation = glm::angleAxis(radianRotation, rotationAxis); }
		inline void setTransparent(bool choice) { m_Transparent = choice; }
		inline void setParent(Renderable3D *parent) { m_Parent = parent; }
	private:
		glm::vec3 m_Position, m_Scale;
		glm::quat m_Orientation;
		Renderable3D *m_Parent;
		std::vector<Renderable3D*> m_Children;

		bool m_Transparent;
		Model *m_Model;
	};

}
