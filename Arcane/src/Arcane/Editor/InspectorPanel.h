#pragma once
#ifndef INSPECTORPANEL_H
#define INSPECTORPANEL_H

#ifndef ENTITY_H
#include <Arcane/Scene/Entity.h>
#endif

namespace Arcane
{
	class InspectorPanel
	{
	public:
		InspectorPanel();

		void OnImGuiRender();

		inline void SetFocusedEntity(Entity entity) { m_FocusedEntity = entity; }
		inline Entity GetFocusedEntity() { return m_FocusedEntity; }
	private:
		bool DrawVec2Control(const std::string &label, glm::vec2 &values, float speed = 0.1f, float min = 0.0f, float max = 0.0f, const char *displayDigits = "%.2f", float columnWidth = 100.0f);
		bool DrawVec3Control(const std::string &label, glm::vec3 &values, float speed = 0.1f, float min = 0.0f, float max = 0.0f, const char *displayDigits = "%.2f", float columnWidth = 100.0f);
		bool DrawFloatControl(const std::string &label, float &value, float speed = 0.1f, float min = 0.0f, float max = 0.0f, const char *displayDigits = "%.2f", float columnWidth = 200.0f);
	private:
		Entity m_FocusedEntity;
	};
}
#endif
