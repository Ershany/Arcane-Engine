#pragma once

namespace Arcane
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame();

		static void RenderMesh();
		// TODO: Fullscreen quad
	};
}
