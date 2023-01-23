#include "arcpch.h"
#include "OpenGLImGuiLayer.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/Vendor/Imgui/imgui.h>
#include <Arcane/Vendor/Imgui/examples/imgui_impl_glfw.h>
#include <Arcane/Vendor/Imgui/examples/imgui_impl_opengl3.h>

namespace Arcane
{
	OpenGLImGuiLayer::OpenGLImGuiLayer(ARC_DEV_ONLY(const std::string &name))
	{
		ARC_DEV_ONLY(m_DebugName = name);
	}

	OpenGLImGuiLayer::~OpenGLImGuiLayer()
	{}

	void OpenGLImGuiLayer::OnAttach()
	{
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		//io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", 18.0f);
		//io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", 24.0f);
		//io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", 18.0f);

		// Setup style
		ImGui::StyleColorsDark();
		//SetDarkThemeColors();
		//ImGui::StyleColorsClassic();

		// When a viewport is created need to check rounding and colour to make it look identical to our window
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, style.Colors[ImGuiCol_WindowBg].w);

		// Setup other bindings
		Application &app = Application::GetInstance();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow()->GetNativeWindow());
		ImGui_ImplGlfw_InitForOpenGL(window, false);
		ImGui_ImplOpenGL3_Init("#version 430");
	}

	void OpenGLImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void OpenGLImGuiLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void OpenGLImGuiLayer::End()
	{
		// Set the display size
		ImGuiIO &io = ImGui::GetIO();
		Application &app = Application::GetInstance();
		io.DisplaySize = ImVec2(float(app.GetWindow()->GetWidth()), float(app.GetWindow()->GetHeight()));

		// Render to window/swap-chain
		Arcane::Window::Bind();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void OpenGLImGuiLayer::OnImGuiRender()
	{

	}
}
