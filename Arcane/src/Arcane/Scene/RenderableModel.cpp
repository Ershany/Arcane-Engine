#include "arcpch.h"
#include "RenderableModel.h"

#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Mesh/Model.h>

namespace Arcane
{
	RenderableModel::RenderableModel(glm::vec3 &position, glm::vec3 &scale, glm::vec3 &rotationAxis, float radianRotation, Model *model, RenderableModel *parent, bool isStatic, bool isTransparent)
		: m_Position(position), m_Scale(scale), m_Orientation(glm::angleAxis(radianRotation, rotationAxis)), m_Model(model), m_Parent(parent), m_IsStatic(isStatic), m_IsTransparent(isTransparent)
	{
	}

	RenderableModel::~RenderableModel() {
		// Recursive delete on children
		for (unsigned int i = 0; i < m_Children.size(); i++) {
			delete m_Children[i];
		}
	}

	void RenderableModel::Draw(Shader *shader, RenderPassType pass) const {
		if(m_Model)
			m_Model->Draw(shader, pass);
	}

	void RenderableModel::AddChild(RenderableModel *child) {
		if (child == this)
			return;

		m_Children.push_back(child);
		child->SetParent(this);
	}
}
