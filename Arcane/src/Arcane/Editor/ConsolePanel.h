#pragma once
#ifndef CONSOLEPANEL_H
#define CONSOLEPANEL_H

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
#endif
