#version 430 core

layout (location = 0) out vec4 gb_Albedo;
layout (location = 1) out vec3 gb_Normal;
layout (location = 2) out vec4 gb_MaterialInfo;

// Does AMD support sampler2D in a struct?
struct Material {
	sampler2D texture_albedo1; // background texture
	sampler2D texture_albedo2; // r texture
	sampler2D texture_albedo3; // g texture
	sampler2D texture_albedo4; // b texture

	sampler2D texture_normal1; // background texture
	sampler2D texture_normal2; // r texture
	sampler2D texture_normal3; // g texture
	sampler2D texture_normal4; // b texture

	sampler2D texture_roughness1; // background texture
	sampler2D texture_roughness2; // r texture
	sampler2D texture_roughness3; // g texture
	sampler2D texture_roughness4; // b texture

	sampler2D texture_metallic1; // background texture
	sampler2D texture_metallic2; // r texture
	sampler2D texture_metallic3; // g texture
	sampler2D texture_metallic4; // b texture

	sampler2D texture_AO1; // background texture
	sampler2D texture_AO2; // r texture
	sampler2D texture_AO3; // g texture
	sampler2D texture_AO4; // b texture

	sampler2D blendmap;
	float tilingAmount;
};

in mat3 TBN;
in vec2 TexCoords;

uniform Material material;

// Functions
vec3 UnpackNormal(vec3 textureNormal);

void main() {
	vec4 blendMapColour = texture(material.blendmap, TexCoords);
	float backTextureWeight = 1 - (blendMapColour.r + blendMapColour.g + blendMapColour.b);
	vec2 tiledCoords = TexCoords * material.tilingAmount;

	vec3 backgroundTextureAlbedo = texture(material.texture_albedo1, tiledCoords).rgb * backTextureWeight;
	vec3 rTextureAlbedo = texture(material.texture_albedo2, tiledCoords).rgb * blendMapColour.r;
	vec3 gTextureAlbedo = texture(material.texture_albedo3, tiledCoords).rgb * blendMapColour.g;
	vec3 bTextureAlbedo = texture(material.texture_albedo4, tiledCoords).rgb * blendMapColour.b;
	vec3 albedo = backgroundTextureAlbedo + rTextureAlbedo + gTextureAlbedo + bTextureAlbedo;

	vec3 backgroundTextureNormal = UnpackNormal(texture(material.texture_normal1, tiledCoords).rgb) * backTextureWeight;
	vec3 rTextureNormal = UnpackNormal(texture(material.texture_normal2, tiledCoords).rgb) * blendMapColour.r;
	vec3 gTextureNormal = UnpackNormal(texture(material.texture_normal3, tiledCoords).rgb) * blendMapColour.g;
	vec3 bTextureNormal = UnpackNormal(texture(material.texture_normal4, tiledCoords).rgb) * blendMapColour.b;
	vec3 normal = normalize(backgroundTextureNormal + rTextureNormal + gTextureNormal + bTextureNormal);

	float backgroundTextureRoughness = texture(material.texture_roughness1, tiledCoords).r * backTextureWeight;
	float rTextureRoughness = texture(material.texture_roughness2, tiledCoords).r * blendMapColour.r;
	float gTextureRoughness = texture(material.texture_roughness3, tiledCoords).r * blendMapColour.g;
	float bTextureRoughness = texture(material.texture_roughness4, tiledCoords).r * blendMapColour.b;
	float roughness = max(max(backgroundTextureRoughness, rTextureRoughness), max(gTextureRoughness, bTextureRoughness));
	roughness = max(roughness, 0.04);

	float backgroundTextureMetallic = texture(material.texture_metallic1, tiledCoords).r * backTextureWeight;
	float rTextureMetallic = texture(material.texture_metallic2, tiledCoords).r * blendMapColour.r;
	float gTextureMetallic = texture(material.texture_metallic3, tiledCoords).r * blendMapColour.g;
	float bTextureMetallic = texture(material.texture_metallic4, tiledCoords).r * blendMapColour.b;
	float metallic = max(max(backgroundTextureMetallic, rTextureMetallic), max(gTextureMetallic, bTextureMetallic));

	float backgroundTextureAO = texture(material.texture_AO1, tiledCoords).r * backTextureWeight;
	float rTextureAO = texture(material.texture_AO2, tiledCoords).r * blendMapColour.r;
	float gTextureAO = texture(material.texture_AO3, tiledCoords).r * blendMapColour.g;
	float bTextureAO = texture(material.texture_AO4, tiledCoords).r * blendMapColour.b;
	float ao = max(max(backgroundTextureAO, rTextureAO), max(gTextureAO, bTextureAO));

	// Normal mapping code. Opted out of tangent space normal mapping since I would have to convert all of my lights to tangent space
	normal = normalize(TBN * UnpackNormal(normal));

	gb_Albedo = vec4(albedo, 1.0);
	gb_Normal = normal;
	gb_MaterialInfo = vec4(metallic, roughness, ao, 1.0);
}

// Unpacks the normal from the texture and returns the normal in tangent space
vec3 UnpackNormal(vec3 textureNormal) {
	return normalize(textureNormal * 2.0 - 1.0);
}
