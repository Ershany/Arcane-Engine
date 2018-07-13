#include "Renderable3D.h"

namespace arcane { namespace graphics {

	Renderable3D::Renderable3D(glm::vec3 &position, glm::vec3 &scale, glm::vec3 &rotationAxis, float radianRotation, Model *model, Renderable3D *parent, bool shouldOutline, bool transparent)
		: m_Position(position), m_Scale(scale), m_Orientation(glm::angleAxis(radianRotation, rotationAxis)), m_Model(model), m_Parent(parent), m_ShouldOutline(shouldOutline), m_Transparent(transparent)
	{
	}

	Renderable3D::~Renderable3D() {
		
	}

	void Renderable3D::draw(Shader &shader) const {
		// TODO: Add toggle in renderable that will skip the texture/material binding stage, which allows the renderer to have even more control
		// Maybe do the binding here in order to easily make this work!
		m_Model->Draw(shader);
	}

} }