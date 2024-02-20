#pragma once

#include "Arcane/Physics/3D/PhysicsDefs.h"


namespace Arcane
{
	class RbBody;
	struct TransformComponent;

	class RbManager
	{
	public:
		RbManager() = default;
		~RbManager();

		void Update(f32 dt);
		void AddRb(RbBody* rb); // TODO: Maybe return the index here
		RbBody* CreateRb(const TransformComponent& trans, phGeometry* geom, bool isDynamic = true); // TODO: Maybe return the index instead
		RbBody* GetRb(u32 index);

	private:
		std::vector<RbBody*> m_Rbs;
	};
}