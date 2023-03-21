#include "arcpch.h"
#include "PhysicsScene.h"
#include "RigidbodyComponent.h"
#include "PhysicsFactory.h"

namespace Arcane
{
	phScene* PhysicsScene::s_Scene;
	phPhysics* PhysicsScene::s_Physics;

	PhysicsScene::PhysicsScene() : m_Pvd(nullptr)
	{
		InitPhysx();
	}

	void PhysicsScene::InitPhysx()
	{
		m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);
		ARC_ASSERT(m_Foundation, "PxCreateFoundation() failed");

#ifdef USE_PHYSX_DEBUGGER
		// This is for debug profiling
		m_Pvd = PxCreatePvd(*m_Foundation);
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		m_Pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
#endif

		// Tolerance scaling assuming meters if you want to use centimeters might not be a bad idea to multiply by 100
		m_ToleranceScale.length = 1.f;        // typical length of an object
		m_ToleranceScale.speed = 9.81;         // typical speed of an object, gravity*1s is a reasonable choice

		// If we want to profile we want to set that bool to true
		s_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, m_ToleranceScale, true, m_Pvd);
		ARC_ASSERT(s_Physics, "PxCreatePhysics failed");

		// Physx extensions if we need them
		PxInitExtensions(*s_Physics, m_Pvd);

		// create a scene
		
		physx::PxSceneDesc sceneDesc(m_ToleranceScale);
		sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);

		physx::PxU32 numCores = physx::PxThread::getNbPhysicalCores();
		m_Dispatcher = physx::PxDefaultCpuDispatcherCreate(numCores == 0 ? 0 : numCores - 1);
		sceneDesc.cpuDispatcher = m_Dispatcher;
		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

		sceneDesc.solverType = physx::PxSolverType::eTGS;
		//sceneDesc.filterShader = scissorFilter;
		s_Scene = s_Physics->createScene(sceneDesc);

		ARC_ASSERT(s_Scene, "scene is nullptr");

		// For loading data in?
		m_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, physx::PxCookingParams(m_ToleranceScale));
		ARC_ASSERT(m_Cooking, "PxCreateCooking Failed");
	}

	PhysicsScene::~PhysicsScene()
	{
		m_Foundation->release();
		s_Physics->release();
		s_Scene->release();

		PxCloseExtensions();
	}

	void PhysicsScene::UpdateSim(f32 dt)
	{
		// TODO: fixed timestep
		s_Scene->simulate(dt);
		s_Scene->fetchResults(false);
	}
}
