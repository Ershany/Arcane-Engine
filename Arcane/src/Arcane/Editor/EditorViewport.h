#pragma once
#ifndef EDITORVIEWPORT_H
#define EDITORVIEWPORT_H

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
#endif
