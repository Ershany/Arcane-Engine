#pragma once
#ifndef VOLUMETRICCLOUDS_H
#define VOLUMETRICCLOUDS_H

namespace Arcane
{
	class Texture3D;

	struct NoiseTextureParams
	{

	};

	class VolumetricClouds
	{
	public:
		// Function for the editor to use to pre-generate a 3D noise texture that can be used for the volumetric cloud effect at runtime
		static Texture3D* Generate3DNoiseTexture(NoiseTextureParams& params);
	private:

	};
}
#endif
