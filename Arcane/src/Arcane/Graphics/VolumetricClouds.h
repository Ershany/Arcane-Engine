#pragma once

namespace Arcane
{
	struct NoiseTextureParams
	{

	};

	class VolumetricClouds
	{
	public:
		static void Generate3DNoiseTexture(NoiseTextureParams& params);
	};
}
