#include "arcpch.h"

#include "Arcane/Scene/Components.h"
#include "RbBody.h"
#include "PhysicsFactory.h"

namespace Arcane
{
	RbBody::RbBody(const TransformComponent& trans, phGeometry* geom, bool isDynamic)
	{
		m_Rb = PhysicsFactory::CreatePhRb(trans, geom, isDynamic);

		// TODO: Change this
#ifdef ARC_DEV_BUILD
		m_Mass = GetMass();
		m_IsKinematic = IsFlagSet(phRbFlag::eKINEMATIC);
		m_IsStatic = !isDynamic;
#endif 
	}

	RbBody::~RbBody()
	{
		if (m_Rb)
		{
			m_Rb->release();
		}
	}

	void RbBody::AddForce(const phVec3& InForce)
	{
		if (IsStatic())
		{
			ARC_ASSERT(false, "The rigidbody is static force should be applied here");
			return;
		}

		// TODO: Not the best approach here
		// TODO: Add the other variables as well
		static_cast<phDynamicRb*>(m_Rb)->addForce(InForce);
	}

	void RbBody::SetMass(f32 InMass)
	{
		// TODO: Not the best here
		static_cast<phRb*>(m_Rb)->setMass(InMass);

		// TODO:
#ifdef ARC_DEV_BUILD
		m_Mass = GetMass();
#endif 
	}

	f32 RbBody::GetMass() const
	{
		return static_cast<phRb*>(m_Rb)->getMass();
	}

	void RbBody::SetFlag(phRbFlag inFlag, bool inVal)
	{
		// TODO: Pretty gross 
		static_cast<phRb*>(m_Rb)->setRigidBodyFlag(static_cast<phRbFlag>(inFlag), inVal);

#ifdef ARC_DEV_BUILD
		m_IsKinematic = IsFlagSet(phRbFlag::eKINEMATIC);
#endif 
	}

	void RbBody::SetFlags(phRbFlags inFlags)
	{
		// TODO: Not gd here. Rethink this
		static_cast<phRb*>(m_Rb)->setRigidBodyFlags(inFlags);

#ifdef ARC_DEV_BUILD
		m_IsKinematic = IsFlagSet(phRbFlag::eKINEMATIC);
#endif 
	}

	phRbFlags RbBody::GetFlags() const
	{
		return static_cast<phRb*>(m_Rb)->getRigidBodyFlags();
	}

	bool RbBody::IsFlagSet(phRbFlag inFlag) const
	{
		phRbFlags& flags = static_cast<phRb*>(m_Rb)->getRigidBodyFlags();
		return flags.isSet(inFlag);
	}
}