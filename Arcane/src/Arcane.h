/*
	File needs to be included by any project that wants to use Arcane
	NEVER INCLUDE THIS FILE ANYWHERE IN THE ENGINE CODEBASE
*/

#pragma once

#include <Arcane/Core/Application.h>
#include <Arcane/Core/Events/Event.h>
#include <Arcane/Core/Layer.h>

#include <Arcane/Graphics/Renderer/Renderer.h>
#include <Arcane/Graphics/Renderer/RenderPass/MasterRenderPass.h>
#include <Arcane/Graphics/Camera/FPSCamera.h>
#include <Arcane/Graphics/Camera/CubemapCamera.h>
#include <Arcane/Graphics/IBL/ProbeManager.h>
#include <Arcane/Graphics/Lights/DynamicLightManager.h>
#include <Arcane/Graphics/Mesh/Material.h>
#include <Arcane/Graphics/Mesh/Model.h>
#include <Arcane/Graphics/Mesh/Mesh.h>
#include <Arcane/Graphics/Texture/Texture.h>
#include <Arcane/Graphics/Texture/Cubemap.h>
#include <Arcane/Graphics/Shader.h>

#include <Arcane/Input/InputManager.h>
#include <Arcane/Input/JoystickManager.h>

#include <Arcane/Scene/Entity.h>
#include <Arcane/Scene/Scene.h>
#include <Arcane/Scene/Components.h>

#include <Arcane/Util/Loaders/AssetManager.h>
#include <Arcane/Util/Logger.h>
#include <Arcane/Util/FileUtils.h>
#include <Arcane/Util/Time.h>
#include <Arcane/Util/Timer.h>
