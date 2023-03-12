#include "arcpch.h"
#include "PhysicsScene.h"

namespace Arcane
{
	PhysicsScene::PhysicsScene()
	{
		m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);
		ARC_ASSERT(m_Foundation, "PxCreateFoundation() failed");

		// This is for debug profiling
		m_Pvd = PxCreatePvd(*m_Foundation);
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		m_Pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

		// Tolerance scaling assuming meters if you want to use centimeters might not be a bad idea to multiply by 100
		m_ToleranceScale.length = 1.f;        // typical length of an object
		m_ToleranceScale.speed = 9.81;         // typical speed of an object, gravity*1s is a reasonable choice

		// If we want to profile we want to set that bool to true
		m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, m_ToleranceScale, false, m_Pvd);
		ARC_ASSERT(m_Physics, "PxCreatePhysics failed");

		// For loading data in?
		m_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, physx::PxCookingParams(m_ToleranceScale));
		ARC_ASSERT(m_Cooking, "PxCreateCooking Failed");

		// Physx extensions if we need them
		ARC_ASSERT(!PxInitExtensions(*m_Physics, m_Pvd), "PxInitExtensions failed");

	}

	PhysicsScene::~PhysicsScene()
	{
		m_Foundation->release();
		m_Physics->release();

		PxCloseExtensions();
	}
}
