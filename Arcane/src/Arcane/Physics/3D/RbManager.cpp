#include "arcpch.h"

#include "RbManager.h"
#include "RbBody.h"
#include "PhysicsFactory.h"
#include "PhysicsScene.h"

namespace Arcane
{
	RbManager::~RbManager()
	{
		for (auto iter = m_Rbs.begin(); iter != m_Rbs.end(); ++iter)
		{
			PHYSICS_SCENE->removeActor(*(*iter)->GetPhRigid()); // TODO: Check if this is expensive
			delete (*iter);
		}
	}

	void RbManager::Update(f32 dt)
	{
		// Physics scene should handle this for now
		for (RbBody* rb : m_Rbs)
		{
			// Physics sim should update the rigidbody components so we shouldn't do this here
		}
	}

	void RbManager::AddRb(RbBody* rb)
	{
		ARC_ASSERT(rb, "Null rigidbody is being added in");
		m_Rbs.push_back(rb);
		PHYSICS_SCENE->addActor(*rb->GetPhRigid());
	}

	RbBody* RbManager::CreateRb(const TransformComponent& trans, phGeometry* geom, bool isDynamic)
	{
		RbBody* body = new RbBody(trans, geom, isDynamic);
		m_Rbs.emplace_back(body);
		PHYSICS_SCENE->addActor(*body->GetPhRigid());
		return body;
	}

	RbBody* RbManager::GetRb(u32 index)
	{
		ARC_ASSERT(m_Rbs[index], "cannot find rigidbody component. It is null");
		return m_Rbs[index];
	}
}
