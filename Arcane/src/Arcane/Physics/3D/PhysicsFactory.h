#pragma once

#include <Arcane/Scene/Components.h>

namespace Arcane
{
	class PhysicsScene;

	class PhysicsFactory
	{
	public:

		static phActorRigid* CreateRigidbody(const TransformComponent& trans, phGeometry* shape, bool isDynamic);

		static phGeometry* CreateSphereGeometry(f32 radius);
		static phGeometry* CreateCapsuleGeometry(f32 radius, f32 halfHeight);
		static phGeometry* CreateBoxGeometry(f32 hx, f32 hy, f32 hz);
		static phGeometry* CreatePlaneGeometry();
		static phConvexMesh* CreateConvexGeometry();
	};
}