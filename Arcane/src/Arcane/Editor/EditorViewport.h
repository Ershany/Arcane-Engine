#pragma once

namespace Arcane
{
	class Texture;

	class EditorViewport
	{
	public:
		EditorViewport();

		void OnImGuiRender();

		bool IsPlaySelected() const { return m_PlaySelected; }
	private:
		Texture *m_PlayTexture, *m_StopTexture, *m_PauseTexture;
		bool m_PlaySelected;
	};
}
