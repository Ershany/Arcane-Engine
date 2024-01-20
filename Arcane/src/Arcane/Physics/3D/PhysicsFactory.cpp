#include "arcpch.h"
#include "PhysicsFactory.h"
#include "PhysicsScene.h"

namespace Arcane
{
	phActorRigid* PhysicsFactory::CreateRigidbody(const TransformComponent& transform, phGeometry* geom, bool isDynamic)
	{
		ARC_ASSERT(geom, "no geometry for rigidbody");
		physx::PxTransform trans = physx::PxTransform(transform.Translation.x, transform.Translation.y, transform.Translation.z, physx::PxQuat(transform.Rotation.x, transform.Rotation.y, transform.Rotation.z, transform.Rotation.w));
		phActorRigid* rb;

		if (isDynamic)
		{
			rb = PhysicsScene::GetPhysics()->createRigidDynamic(trans);
		}
		else
		{
			rb = PhysicsScene::GetPhysics()->createRigidStatic(trans);
		}

		// TODO: Material data 
		phMaterial* material = PhysicsScene::GetPhysics()->createMaterial(0.f, 0.f, 0.f);
		phShape* shape = PhysicsScene::GetPhysics()->createShape(*geom, *material);
		shape->setGeometry(*geom);
		rb->attachShape(*shape);

		// maybe
		shape->release(); // ref counting

		return rb;
	}

	phGeometry* PhysicsFactory::CreateSphereGeometry(f32 radius)
	{
		return new phSphereGeometry(radius);
	}

	phGeometry* PhysicsFactory::CreateCapsuleGeometry(f32 radius, f32 halfHeight)
	{
		// Capsule are created with x-axis being the direction it is spawned in. We will need to rotate the by half PI along the z-axis to get it to the Y-axis
		// Something like this: 
		// PxTransform relativePose(PxQuat(PxHalfPi, PxVec(0,0,1)));
		return new phCapsuleGeometry(radius, halfHeight);
	}

	phGeometry* PhysicsFactory::CreateBoxGeometry(f32 hx, f32 hy, f32 hz)
	{
		// Boxes are created with x-axis being the direction it is spawned in. We will need to rotate the by half PI along the z-axis to get it to the Y-axis
		// Something like this: 
		// PxTransform relativePose(PxQuat(PxHalfPi, PxVec(0,0,1)));
		return new phBoxGeometry(hx, hy, hz);
	}

	phGeometry* PhysicsFactory::CreatePlaneGeometry()
	{
		// Planes are created with x-axis being the direction it is spawned in. We will need to rotate the by half PI along the z-axis to get it to the Y-axis
		// Something like this: 
		// PxTransform relativePose(PxQuat(PxHalfPi, PxVec(0,0,1)));
		// Everything about plane is based on the actor itself
		return new phPlaneGeometry();
	}

	phConvexMesh* PhysicsFactory::CreateConvexGeometry()
	{
		ARC_ASSERT(false, "CreateConvexGeometry() has not been implemented yet.");

		// TODO: This will need more work so will do this later
		//phConvexMesh* convex;
		return nullptr;
	}
}