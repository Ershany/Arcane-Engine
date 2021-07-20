#pragma once

#include <Arcane/Graphics/Renderer/Renderpass/RenderPassType.h>

namespace Arcane
{
	class Model;
	class Shader;

	class RenderableModel {
	public:
		RenderableModel(glm::vec3 &position, glm::vec3 &scale, glm::vec3 &rotationAxis, float radianRotation, Model *model, RenderableModel *parent, bool isStatic = false, bool isTransparent = false);
		~RenderableModel();

		// Assumes shader is already bound by the renderer
		void Draw(Shader *shader, RenderPassType pass) const;

		void AddChild(RenderableModel *child);


		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline const glm::vec3& GetScale() const { return m_Scale; }
		inline const glm::quat& GetOrientation() const { return m_Orientation; }
		inline const RenderableModel* GetParent() const { return m_Parent; }
		inline bool GetTransparent() const { return m_IsTransparent; }
		inline bool GetStatic() const { return m_IsStatic; }
		inline const Model* GetModel() const { return m_Model; }

		inline void SetPosition(glm::vec3 &other) { m_Position = other; }
		inline void SetScale(glm::vec3 &other) { m_Scale = other; }
		inline void SetOrientation(float radianRotation, glm::vec3 rotationAxis) { m_Orientation = glm::angleAxis(radianRotation, rotationAxis); }
		inline void SetTransparent(bool choice) { m_IsTransparent = choice; }
		inline void SetParent(RenderableModel *parent) { m_Parent = parent; }
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
