#pragma once

namespace Arcane
{
	class ConsolePanel
	{
	public:
		ConsolePanel();

		void OnImGuiRender();
	private:
		void RenderMenu();
		void RenderConsole();
	};
}
