#pragma once

#include "arcpch.h"

#include <Arcane/Graphics/Mesh/Model.h>

namespace Arcane
{
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

		glm::mat4 GetTransform()
		{
			return glm::translate(glm::mat4(1.0f), Translation)
				* glm::toMat4(glm::quat(Rotation))
				* glm::scale(glm::mat4(1.0f), Scale);
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
	};

	// TODO: Eventually needs to be added and used for the runtime. The editor will always have a "camera" but will be needed for runtime eventually
	struct CameraComponent
	{
		Arcane::ICamera *camera;
	};
}
