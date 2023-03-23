#pragma once

#include "RigidbodyComponent.h"
namespace Arcane
{
	class RigidbodyManager
	{
	public:
		RigidbodyManager();
		~RigidbodyManager();

		void Update(f32 dt);
		void AddRigidbodyComponent(RigidbodyComponent* rb);
		void AddRigidbodyComponent(const TransformComponent& trans, phGeometry* geom, bool isDynamic);
		//void AddRigidbodyComponent(const TransformComponent& trans, phGeometryType geom, bool isDynamic);
		RigidbodyComponent* GetRigidbodyComponent(u32 index);

	private:
		std::vector<RigidbodyComponent*> m_Rbs;

	};
}