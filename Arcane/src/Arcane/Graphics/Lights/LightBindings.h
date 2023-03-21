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

		const static int MaxDirLights = 3;
		const static int MaxPointLights = 6;
		const static int MaxSpotLights = 6;
	};
}
