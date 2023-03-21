#pragma once

namespace Arcane
{
	class PhysicsScene
	{
	public:
		PhysicsScene();
		~PhysicsScene();

		void UpdateSim(f32 dt);

		inline static phScene* GetScene() { return s_Scene; }
		inline static phPhysics* GetPhysics() { return s_Physics; }

	private:
		void InitPhysx();

		physx::PxDefaultAllocator m_Allocator;
		physx::PxDefaultErrorCallback m_ErrorCallback; // Could be smthg we can hook into using our logger will need to implement  a PxErrorCallback::reportError()
		physx::PxFoundation* m_Foundation;
		static phPhysics* s_Physics;
		physx::PxTolerancesScale m_ToleranceScale;
		physx::PxCooking* m_Cooking;
		physx::PxDefaultCpuDispatcher* m_Dispatcher;
		static phScene* s_Scene;

		physx::PxPvd* m_Pvd;
	};
}
