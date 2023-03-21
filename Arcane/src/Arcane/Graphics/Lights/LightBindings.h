#pragma once

namespace Arcane
{
	class Shader;
	struct TransformComponent;
	struct LightComponent;

	class LightBindings
	{
	public:
		static void BindDirectionalLight(const TransformComponent &tranformComponent, const LightComponent &lightComponent, Shader *shader, int currentLightIndex);
		static void BindPointLight(const TransformComponent &tranformComponent, const LightComponent &lightComponent, Shader *shader, int currentLightIndex);
		static void BindSpotLight(const TransformComponent &tranformComponent, const LightComponent &lightComponent, Shader *shader, int currentLightIndex);
	};
}
