#pragma once
#ifndef COMPONENTS_H
#define COMPONENTS_H

#ifndef ARCPCH_H
#include "arcpch.h"
#endif

#ifndef LIGHTMANAGER_H
#include <Arcane/Graphics/Lights/LightManager.h>
#endif

#ifndef WATERMANAGER_H
#include <Arcane/Graphics/Water/WaterManager.h>
#endif

#ifndef POSEANIMATOR_H
#include <Arcane/Animation/PoseAnimator.h>
#endif

#ifndef TEXTURE_H
#include <Arcane/Graphics/Texture/Texture.h>
#endif

#ifndef TEXTURE3D_H
#include <Arcane/Graphics/Texture/Texture3D.h>
#endif

namespace Arcane
{
	class ICamera;
	class Model;

	struct TagComponent
	{
		std::string Tag;
	
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string &name) : Tag(name) {}

		operator std::string& () { return Tag; }
		operator const std::string& () const { return Tag; }
	};

	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		glm::vec3 Up = { 0.0f, 1.0f, 0.0f };
		glm::vec3 Right = { 1.0f, 0.0f, 0.0f };
		glm::vec3 Forward = { 0.0f, 0.0f, -1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent &other) = default;
		TransformComponent(const glm::vec3 &translation) : Translation(translation)
		{
		}

		glm::mat4 GetTransform() const
		{
			return glm::translate(glm::mat4(1.0f), Translation)
				* glm::toMat4(glm::quat(Rotation))
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		glm::vec3 GetForward() const
		{
			return (GetTransform() * glm::vec4(Forward, 0.0f)).xyz();
		}

		glm::vec3 GetRight() const
		{
			return (GetTransform() * glm::vec4(Right, 0.0f)).xyz();
		}

		glm::vec3 GetUp() const
		{
			return (GetTransform() * glm::vec4(Up, 0.0f)).xyz();
		}
	};

	struct MeshComponent
	{
		Model *AssetModel;

		MeshComponent() = default;
		MeshComponent(const MeshComponent &other) = default;
		MeshComponent(Model *otherModel) : AssetModel(otherModel)
		{
		}

		bool IsTransparent = false; // Should be true if the model contains any translucent material
		bool IsStatic = false;		// Should be true if the model will never have its transform modified
		bool ShouldBackfaceCull = true; // Should be true for majority of models, unless a model isn't double sided
	};

	struct LightComponent
	{
		LightType Type = LightType::LightType_Point;

		float Intensity = 1.0f;
		glm::vec3 LightColour = glm::vec3(1.0f, 1.0f, 1.0f);
		float AttenuationRange = 10.0f; // Used for spot and point lights only

		// Used for spot lights and stored as [0, 1] as cos(angle)
		float InnerCutOff = glm::cos(glm::radians(25.0f));
		float OuterCutOff = glm::cos(glm::radians(27.5f));

		bool IsStatic = false; // Should be true if the light will never have any of it's properties changed (then we can use it for global illumination since it is static)

		bool CastShadows = false;
		float ShadowBias = SHADOWMAP_BIAS_DEFAULT;
		ShadowQuality ShadowResolution = ShadowQuality::ShadowQuality_Medium;
		float ShadowNearPlane = SHADOWMAP_NEAR_PLANE_DEFAULT, ShadowFarPlane = SHADOWMAP_FAR_PLANE_DEFAULT;
	};

	struct PoseAnimatorComponent
	{
		PoseAnimator PoseAnimator;
	};

	struct WaterComponent
	{
		glm::vec3 WaterAlbedo = glm::vec3(0.102f, 0.431f, 0.902f);
		float AlbedoPower = 0.05f;

		bool ReflectionEnabled = true;
		WaterReflectionRefractionQuality WaterRefractionResolution = WaterReflectionRefractionQuality::WaterReflectionRefractionQuality_High;
		bool RefractionEnabled = true;
		WaterReflectionRefractionQuality WaterReflectionResolution = WaterReflectionRefractionQuality::WaterReflectionRefractionQuality_High;

		bool ReflectionMSAA = true;
		bool RefractionMSAA = false;
		bool ClearWater = false;
		bool EnableShine = true;

		glm::vec2 WaterTiling = glm::vec2(6.0f, 6.0f);
		float WaveSpeed = 0.05f;
		float WaveStrength = 0.02f;

		float ShineDamper = 300.0f;
		float NormalSmoothing = 3.0f;
		float DepthDampening = 0.05f;

		float ReflectionPlaneBias = 1.5f;
		float RefractionPlaneBias = 0.5f;
		
		float ReflectionNearPlane = WATER_REFLECTION_NEAR_PLANE_DEFAULT, ReflectionFarPlane = WATER_REFLECTION_FAR_PLANE_DEFAULT;
		float RefractionNearPlane = WATER_REFRACTION_NEAR_PLANE_DEFAULT, RefractionFarPlane = WATER_REFRACTION_FAR_PLANE_DEFAULT;

		float MoveTimer = 0.0f; // Should not be set or used by the user. Just used for water rendering, that is why this isn't viewable/modifiable in the inspector panel

		Texture *WaterDistortionTexture = nullptr;
		Texture *WaterNormalMap = nullptr;
	};

	// TODO: Eventually needs to be added and used for the runtime. The editor will always have a "camera" but will be needed for runtime eventually
	struct CameraComponent
	{
		Arcane::ICamera *camera;
	};

	struct VolumetricCloudComponent
	{
		Texture3D* GeneratedNoiseTexture3D = nullptr;
	};
}
#endif

