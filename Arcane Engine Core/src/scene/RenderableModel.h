#pragma once

#include <graphics/mesh/Model.h>
#include <graphics/renderer/renderpass/RenderPassType.h>

namespace arcane {

	class RenderableModel {
	public:
		RenderableModel(glm::vec3 &position, glm::vec3 &scale, glm::vec3 &rotationAxis, float radianRotation, Model *model, RenderableModel *parent, bool isStatic = false, bool isTransparent = false);
		~RenderableModel();

		// Assumes shader is already bound by the renderer
		void draw(Shader *shader, RenderPassType pass) const;

		void addChild(RenderableModel *child);


		inline const glm::vec3& getPosition() const { return m_Position; }
		inline const glm::vec3& getScale() const { return m_Scale; }
		inline const glm::quat& getOrientation() const { return m_Orientation; }
		inline const RenderableModel* getParent() const { return m_Parent; }
		inline bool getTransparent() const { return m_IsTransparent; }
		inline bool getStatic() const { return m_IsStatic; }

		inline void setPosition(glm::vec3 &other) { m_Position = other; }
		inline void setScale(glm::vec3 &other) { m_Scale = other; }
		inline void setOrientation(float radianRotation, glm::vec3 rotationAxis) { m_Orientation = glm::angleAxis(radianRotation, rotationAxis); }
		inline void setTransparent(bool choice) { m_IsTransparent = choice; }
		inline void setParent(RenderableModel *parent) { m_Parent = parent; }
	private:
		// Transformation data
		glm::vec3 m_Position, m_Scale;
		glm::quat m_Orientation;

		RenderableModel *m_Parent;
		std::vector<RenderableModel*> m_Children;
		Model *m_Model;

		bool m_IsTransparent; // Should be true if the model contains any translucent material
		bool m_IsStatic;	  // Should be true if the model will never have its transform modified
	};

}
