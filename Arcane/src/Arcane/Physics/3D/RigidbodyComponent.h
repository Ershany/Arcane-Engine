#pragma once
#include "PhysicsScene.h"
#include "Arcane/Scene/Components.h"

namespace Arcane
{
	class RigidbodyComponent
	{
	public:
		RigidbodyComponent() : m_Rigidbody(nullptr) {}
		RigidbodyComponent(const TransformComponent& trans, phGeometry* geom, bool isDynamic);

		//RigidbodyComponent(const TransformComponent& trans, phGeometryType::Enum geomType, bool isDynamic);
		~RigidbodyComponent();


		phActorRigid* GetRigidbody() { return m_Rigidbody; };
	
		// Might be smthg to extend possibly
		inline bool isStatic() { return m_Rigidbody->getType() == phActorType::eRIGID_STATIC; }

	private:
		phActorRigid* m_Rigidbody;
	};
}