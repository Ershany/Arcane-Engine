#include "pch.h"
#include <scene/Renderable3D.h>

namespace arcane {

	Renderable3D::Renderable3D(glm::vec3 &position, glm::vec3 &scale, glm::vec3 &rotationAxis, float radianRotation, Model *model, Renderable3D *parent, bool transparent)
		: m_Position(position), m_Scale(scale), m_Orientation(glm::angleAxis(radianRotation, rotationAxis)), m_Model(model), m_Parent(parent), m_Transparent(transparent)
	{
	}

	Renderable3D::~Renderable3D() {
		// Recursive delete on children
		for (unsigned int i = 0; i < m_Children.size(); i++) {
			delete m_Children[i];
		}
	}

	void Renderable3D::draw(Shader &shader, RenderPass pass) const {
		if(m_Model)
			m_Model->Draw(shader, pass);
	}

	void Renderable3D::addChild(Renderable3D *child) {
		if (child == this)
			return;

		m_Children.push_back(child);
		child->setParent(this);
	}

}
