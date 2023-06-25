#include "arcpch.h"
#include "LightBindings.h"

#include <Arcane/Scene/Components.h>
#include <Arcane/Graphics/Shader.h>

namespace Arcane
{
	void LightBindings::BindDirectionalLight(const TransformComponent &transformComponent, const LightComponent &lightComponent, Shader *shader, int currentLightIndex)
	{
		ARC_ASSERT(currentLightIndex < MaxDirLights, "Exceeded Directional Light Count");
		shader->SetUniform(("dirLights[" + std::to_string(currentLightIndex) + "].direction").c_str(), transformComponent.GetForward());
		shader->SetUniform(("dirLights[" + std::to_string(currentLightIndex) + "].intensity").c_str(), lightComponent.Intensity);
		shader->SetUniform(("dirLights[" + std::to_string(currentLightIndex) + "].lightColour").c_str(), lightComponent.LightColour);
	}

	void LightBindings::BindPointLight(const TransformComponent &transformComponent, const LightComponent &lightComponent, Shader *shader, int currentLightIndex)
	{
		ARC_ASSERT(currentLightIndex < MaxPointLights, "Exceeded Point Light Count");
		shader->SetUniform(("pointLights[" + std::to_string(currentLightIndex) + "].position").c_str(), transformComponent.Translation);
		shader->SetUniform(("pointLights[" + std::to_string(currentLightIndex) + "].intensity").c_str(), lightComponent.Intensity);
		shader->SetUniform(("pointLights[" + std::to_string(currentLightIndex) + "].lightColour").c_str(), lightComponent.LightColour);
		shader->SetUniform(("pointLights[" + std::to_string(currentLightIndex) + "].attenuationRadius").c_str(), lightComponent.AttenuationRange);
	}

	void LightBindings::BindSpotLight(const TransformComponent &transformComponent, const LightComponent &lightComponent, Shader *shader, int currentLightIndex)
	{
		ARC_ASSERT(currentLightIndex < MaxSpotLights, "Exceeded Spot Light Count");
		shader->SetUniform(("spotLights[" + std::to_string(currentLightIndex) + "].position").c_str(), transformComponent.Translation);
		shader->SetUniform(("spotLights[" + std::to_string(currentLightIndex) + "].direction").c_str(), transformComponent.GetForward());
		shader->SetUniform(("spotLights[" + std::to_string(currentLightIndex) + "].intensity").c_str(), lightComponent.Intensity);
		shader->SetUniform(("spotLights[" + std::to_string(currentLightIndex) + "].lightColour").c_str(), lightComponent.LightColour);
		shader->SetUniform(("spotLights[" + std::to_string(currentLightIndex) + "].attenuationRadius").c_str(), lightComponent.AttenuationRange);
		shader->SetUniform(("spotLights[" + std::to_string(currentLightIndex) + "].cutOff").c_str(), lightComponent.InnerCutOff);
		shader->SetUniform(("spotLights[" + std::to_string(currentLightIndex) + "].outerCutOff").c_str(), lightComponent.OuterCutOff);
	}
}
