#pragma once

#include <Arcane.h>

#include <Arcane/Editor/ConsolePanel.h>
#include <Arcane/Editor/EditorViewport.h>
#include <Arcane/Editor/GraphicsSettings.h>
#include <Arcane/Editor/InspectorPanel.h>
#include <Arcane/Editor/ScenePanel.h>
#include <Arcane/Editor/RendererStatsDisplay.h>

namespace Arcane
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer(const std::string &debugName = "Layer");
		virtual ~EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float deltaTime) override;

		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event &event) override;

		void NewScene();
		void OpenScene(const std::string& filepath);
	private:
		Scene *m_EditorScene;

		EditorViewport m_EditorViewport;
		ConsolePanel m_ConsolePanel;
		ScenePanel m_ScenePanel;
		InspectorPanel m_InspectorPanel;
		RendererStatsDisplay m_RendererStatsDisplay;

		GraphicsSettings m_GraphicsSettings;

		bool m_ShowGraphicsSettings;
	};
}
