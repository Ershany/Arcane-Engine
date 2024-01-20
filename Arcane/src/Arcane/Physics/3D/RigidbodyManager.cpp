#include "arcpch.h"
#include "RigidbodyManager.h"
#include "PhysicsFactory.h"

namespace Arcane
{

	RigidbodyManager::RigidbodyManager()
	{}

	RigidbodyManager::~RigidbodyManager()
	{
		for (auto iter = m_Rbs.begin(); iter != m_Rbs.end(); ++iter)
		{
			delete (*iter);
		}
	}

	void RigidbodyManager::Update(f32 dt)
	{
		// Physics scene should handle this for now
		for (RigidbodyComponent* rb : m_Rbs)
		{
		}
	}

	void RigidbodyManager::AddRigidbodyComponent(RigidbodyComponent* rb)
	{
		ARC_ASSERT(rb, "Null rigidbody is being added in");
		m_Rbs.push_back(rb);
	}

	void RigidbodyManager::AddRigidbodyComponent(const TransformComponent& trans, phGeometry* geom, bool isDynamic)
	{
		RigidbodyComponent* comp = new RigidbodyComponent(trans, geom, isDynamic);
		m_Rbs.emplace_back(comp);
	}

	RigidbodyComponent* RigidbodyManager::GetRigidbodyComponent(u32 index)
	{
		ARC_ASSERT(m_Rbs[index], "cannot find rigidbody component. It is null");
		return m_Rbs[index];
	}
}
