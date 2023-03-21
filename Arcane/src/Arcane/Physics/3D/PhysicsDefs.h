#pragma once

#include <PxPhysicsAPI.h>

namespace Arcane
{
	// Rigidbody classes
	typedef physx::PxBase phBase;
	typedef physx::PxRigidActor phActorRigid;
	typedef physx::PxRigidBody phRb;
	typedef physx::PxRigidDynamic phDynamicRb;
	typedef physx::PxRigidStatic phStaticRb;

	typedef physx::PxActorType phActorType;

	typedef physx::PxShape phShape;

	// Physics geometric shapes
	typedef physx::PxGeometry phGeometry;
	typedef physx::PxSphereGeometry phSphereGeometry;
	typedef physx::PxCapsuleGeometry phCapsuleGeometry;
	typedef physx::PxBoxGeometry phBoxGeometry;
	typedef physx::PxPlaneGeometry phPlaneGeometry;
	typedef physx::PxConvexMesh phConvexMesh;
	typedef physx::PxGeometryType phGeometryType;

	// Material qualities
	typedef physx::PxMaterial phMaterial;

	// physics scene
	typedef physx::PxScene phScene;

	// 
	typedef physx::PxPhysics phPhysics;

}