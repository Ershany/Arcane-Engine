#pragma once


namespace Arcane
{
	struct TransformComponent;

	class RbBody
	{
	public:
		RbBody() : m_Rb(nullptr) {}
		RbBody(const TransformComponent& trans, phGeometry* geom, bool isDynamic);

		~RbBody();

		inline phActorRigid* GetPhRigid() { return m_Rb; };

		void AddForce(const phVec3& inForce);
		// void AddTorque();
		void SetMass(f32 inMass);
		f32 GetMass() const;
		void SetFlag(phRbFlag inFlag, bool inVal); // Change 1 flag
		void SetFlags(phRbFlags inFlags); // Override Rb flags
		phRbFlags GetFlags() const;
		bool IsFlagSet(phRbFlag inFlag) const;

		// Might be smthg to extend possibly
		inline bool IsStatic() const { return m_Rb->getType() == phActorType::eRIGID_STATIC; }

		// For UI (Change this once we have an editor definition)
#ifdef ARC_DEV_BUILD
		bool& GetIsKinematicRef() { return m_IsKinematic; }
		f32& GetMassRef() { return m_Mass; }
		bool& GetIsStaticRef() { return m_IsStatic; }
#endif

	private:
		phActorRigid* m_Rb;

		// For UI (Change this once we have an editor definition)
#ifdef ARC_DEV_BUILD
		bool m_IsKinematic;
		f32 m_Mass;
		bool m_IsStatic;
#endif
	};
}