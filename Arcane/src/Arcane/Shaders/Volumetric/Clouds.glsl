#shader-type vertex
#version 430 core

layout (location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 inverseModel;
uniform vec3 cameraPosition;
uniform vec3 scale;

out vec3 LocalPosition;
out vec3 CameraToFragDirLocal;

void main()
{
	vec3 worldPosition = vec3(model * vec4(position, 1.0f));
	vec3 cameraToFragDir = normalize(worldPosition - cameraPosition); // TODO: Instead of cameraPosition, use the view matrix to get the camera position (view[3].xyz))

	LocalPosition = position * scale;
	CameraToFragDirLocal = vec3(inverseModel * vec4(cameraToFragDir, 1.0));

	gl_Position = projection * view * vec4(worldPosition, 1.0);
}




#shader-type fragment
#version 430 core

in vec3 LocalPosition;
in vec3 CameraToFragDirLocal;

out vec4 FragColour;

uniform sampler3D noiseTexture3D;
uniform vec3 scale;

#define MARCH_SIZE 0.08f
#define MAX_STEPS 100

void main()
{
	float cloudStrength = 0.5f; // TODO: Convert to uniform and make configureable on the entity
	vec3 halfSize = vec3(scale.x * 0.5f, scale.y * 0.5f, scale.z * 0.5f);

	vec3 currPosLocal = LocalPosition;
	float density = 0.0f;
	for (int i = 0; i < MAX_STEPS; ++i)
	{
		// Normalize the local position to the [0, 1] range for texture sampling
		vec3 texCoord = (currPosLocal + halfSize) / scale;
		float noiseSample = texture(noiseTexture3D, texCoord).r;
		//float noiseSample = texture(noiseTexture3D, currPosLocal).r;

		density += noiseSample * MARCH_SIZE * cloudStrength;
		if (density > 1.0f)
		{
			break;
		}

		currPosLocal += CameraToFragDirLocal * MARCH_SIZE;
		if (currPosLocal.x < -halfSize.x || currPosLocal.x > halfSize.x ||
			currPosLocal.y < -halfSize.y || currPosLocal.y > halfSize.y ||
			currPosLocal.z < -halfSize.z || currPosLocal.z > halfSize.z)
		{
			break;
		}
		/*
		if (currPosLocal.x < -1.0f || currPosLocal.x > 1.0f ||
			currPosLocal.y < -1.0f || currPosLocal.y > 1.0f ||
			currPosLocal.z < -1.0f || currPosLocal.z > 1.0f)
		{
			break;
		}
		*/
	}

	float densityClamped = clamp(density, 0.0f, 1.0f);
	FragColour = vec4(vec3(densityClamped), densityClamped);
}
