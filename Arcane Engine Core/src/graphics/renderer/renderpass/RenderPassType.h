#pragma once

namespace arcane
{

	enum RenderPassType
	{
		ShadowmapPass,
		LightingPass,
		PostProcessPass
	};

	struct ShadowmapPassOutput
	{
		glm::mat4 directionalLightViewProjMatrix;
		unsigned int shadowmapTexture;
	};

	struct LightingPassOutput
	{
		unsigned int outputTexture;
	};

}
