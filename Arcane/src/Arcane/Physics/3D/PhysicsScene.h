#pragma once

namespace Arcane
{
	class PhysicsScene
	{
	public:
		PhysicsScene();
		~PhysicsScene();

	private:
		physx::PxDefaultAllocator m_Allocator;
		physx::PxDefaultErrorCallback m_ErrorCallback; // Could be smthg we can hook into using our logger will need to implement  a PxErrorCallback::reportError()
		physx::PxFoundation* m_Foundation;
		physx::PxPhysics* m_Physics;
		physx::PxTolerancesScale m_ToleranceScale;
		physx::PxCooking* m_Cooking;
		physx::PxDefaultCpuDispatcher* m_Dispatcher;
		physx::PxScene* m_Scene;
		physx::PxMaterial* m_Material;
		physx::PxPvd* m_Pvd;
	};
}
