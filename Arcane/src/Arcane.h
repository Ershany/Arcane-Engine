/*
	File needs to be included by any project that wants to use Arcane
	NEVER INCLUDE THIS FILE ANYWHERE IN THE ENGINE CODEBASE
*/

#pragma once

#include <Core/Application.h>
#include <Core/events/Event.h>

#include <Graphics/camera/FPSCamera.h>
#include <Graphics/camera/CubemapCamera.h>
#include <Graphics/ibl/ProbeManager.h>
#include <Graphics/lights/DynamicLightManager.h>
#include <Graphics/mesh/Material.h>
#include <Graphics/mesh/Model.h>
#include <Graphics/mesh/Mesh.h>
#include <Graphics/texture/Texture.h>
#include <Graphics/texture/Cubemap.h>
#include <Graphics/Shader.h>

#include <Input/InputManager.h>
#include <Input/JoystickManager.h>

#include <Scene/Scene3D.h>

#include <Utils/Logger.h>
#include <Utils/FileUtils.h>
#include <Utils/Time.h>
#include <Utils/Timer.h>
