#version 450 core

// Does AMD support sampler2D in a struct?
struct Material {
	sampler2D texture_diffuse1; // background texture
	sampler2D texture_diffuse2; // r texture
	sampler2D texture_diffuse3; // g texture
	sampler2D texture_diffuse4; // b texture
	sampler2D texture_diffuse5; // blend map
	float shininess;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform Material material;
uniform vec3 viewPos;

void main() {
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec4 blendMapColour = texture(material.texture_diffuse5, TexCoords);
	
	float backTextureAmount = 1 - (blendMapColour.r + blendMapColour.g + blendMapColour.b);
	//vec2 tiledCoords = 
	vec4 backgroundTextureColour = texture(material.texture_diffuse1, TexCoords * 128) * backTextureAmount; // Should be TileCoords
	vec4 rTextureColour = texture(material.texture_diffuse2, TexCoords * 128) * blendMapColour.r; // Should be TileCoords
	vec4 gTextureColour = texture(material.texture_diffuse3, TexCoords * 128) * blendMapColour.g; // Should be TileCoords
	vec4 bTextureColour = texture(material.texture_diffuse4, TexCoords * 128) * blendMapColour.b; // Should be TileCoords
	
	vec4 terrainColour = backgroundTextureColour + rTextureColour + gTextureColour + bTextureColour;
	
	
	// Result
	color = terrainColour;
}