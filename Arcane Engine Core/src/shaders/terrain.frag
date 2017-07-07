#version 450 core

// Does AMD support sampler2D in a struct?
struct Material {
	float shininess;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform Material material;
uniform vec3 viewPos;

uniform sampler2D backgroundTexture;
uniform sampler2D rTexture;
uniform sampler2D gTexture;
uniform sampler2D bTexture;
uniform sampler2D blendMap;

void main() {
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	//vec4 blendMapColour = texture(blendMap, TexCoords);
	//
	//float backTextureAmount = 1 - (blendMapColour.r + blendMapColour.g + blendMapColour.b);
	////vec2 tiledCoords = 
	//vec4 backgroundTextureColour = texture(backgroundTexture, tiledCoords) * backTextureAmount;
	//vec4 rTextureColour = texture(rTexture, tiledCoords) * blendMapColour.r;
	//vec4 gTextureColour = texture(gTexture, tiledCoords) * blendMapColour.g;
	//vec4 bTextureColour = texture(bTexture, tiledCoords) * blendMapColour.b;
	//
	//// Result
	//vec4 terrainColour = backroundTextureColour + rTextureColour + gTextureColour + bTextureColour;
	//
	//
	//// Result
	//color = terrainColour;

	color = vec4(1.0, 0.0, 1.0, 0.0);
}