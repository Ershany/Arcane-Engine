#include "arcpch.h"
#include "RigidbodyComponent.h"
#include "PhysicsFactory.h"

namespace Arcane
{

	RigidbodyComponent::RigidbodyComponent(const TransformComponent& trans, phGeometry* geom, bool isDynamic)
	{
		m_Rigidbody = PhysicsFactory::CreateRigidbody(trans, geom, isDynamic);
		PhysicsScene::GetScene()->addActor(*m_Rigidbody);
	}

	//RigidbodyComponent::RigidbodyComponent(const TransformComponent& trans, phGeometryType::Enum geomType, bool isDynamic)
	//{
	//	switch (geomType)
	//	{
	//		case phGeometryType::eSPHERE
	//		{
	//			PhysicsScene::CreateSphereGeometry()
	//			break;
	//		}
	//		case phGeometryType::eBOX
	//		{
	//			break;
	//		}
	//		case phGeometryType::eCAPSULE
	//		{
	//			break;
	//		}
	//		case phGeometryType::ePLANE
	//		{
	//			break;
	//		}
	//		case phGeometryType::eCONVEXMESH
	//		{
	//			break;
	//		}
	//		default:
	//			ARC_ASSERT(false "Non-supported geometry type is being requested");
	//	}
	//}

	RigidbodyComponent::~RigidbodyComponent()
	{
		if (m_Rigidbody)
		{
			m_Rigidbody->release();
		}
	}

}