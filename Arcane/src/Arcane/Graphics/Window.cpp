#include "arcpch.h"
#include "Window.h"

#include <Arcane/Vendor/Imgui/examples/imgui_impl_glfw.h>

namespace Arcane
{
	// Static declarations
	bool Window::s_HideCursor;
	bool Window::s_HideUI;
	int Window::s_Width; int Window::s_Height;
	int Window::s_RenderResolutionWidth; int Window::s_RenderResolutionHeight;
	bool Window::s_VSync;
	bool Window::s_EnableImGui;

	Window::Window(Application *application, const ApplicationSpecification &specification)
		: m_Application(application), m_Title(specification.Name.c_str())
	{
		s_Width = specification.WindowWidth;
		s_Height = specification.WindowHeight;
		s_RenderResolutionWidth = specification.RenderResolutionWidth;
		s_RenderResolutionHeight = specification.RenderResolutionHeight;
		s_VSync = specification.VSync;
		s_HideCursor = true;
		s_HideUI = false;
		s_EnableImGui = specification.EnableImGui;
	}

	Window::~Window() {
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void Window::Init()
	{
		if (!InitInternal())
		{
			ARC_LOG_FATAL("Failed to initialize window");
			glfwDestroyWindow(m_Window);
			glfwTerminate();
		}
	}

	bool Window::InitInternal() {
		// Needed in order to establish the correct OpenGL context (also enabled the usage of RenderDoc along with the window hints)
		glewExperimental = true;

		if (!glfwInit()) {
			ARC_LOG_FATAL("Failed to initialize GLFW window");
			return false;
		}

		// Context hints
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

		// Window hints
		glfwWindowHint(GLFW_DOUBLEBUFFER, true);

		// Create the window and OpenGL context
		if (FULLSCREEN_MODE) {
			SetFullscreenResolution();
			m_Window = glfwCreateWindow(s_Width, s_Height, m_Title, glfwGetPrimaryMonitor(), NULL);
		}
		else {
			m_Window = glfwCreateWindow(s_Width, s_Height, m_Title, NULL, NULL);
		}
		
		if (!m_Window) {
			ARC_LOG_FATAL("Failed to initialize GLFW window");
			return false;
		}

		// Setup the mouse settings
		if (s_HideCursor) 
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		double currMouseX, currMouseY;
		glfwGetCursorPos(m_Window, &currMouseX, &currMouseY);
		g_InputManager.SetMousePos(currMouseX, currMouseY);

		// Set up contexts and callbacks
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, this);
		glfwSetErrorCallback(error_callback);
		glfwSetWindowCloseCallback(m_Window, window_close_callback);
		glfwSetWindowSizeCallback(m_Window, window_resize_callback);
		glfwSetFramebufferSizeCallback(m_Window, framebuffer_resize_callback);
		glfwSetCursorPosCallback(m_Window, cursor_position_callback);
		glfwSetJoystickCallback(joystick_callback);
		if (s_EnableImGui)
		{
			glfwSetKeyCallback(m_Window, key_callback_imgui);
			glfwSetMouseButtonCallback(m_Window, mouse_button_callback_imgui);
			glfwSetScrollCallback(m_Window, scroll_callback_imgui);
			glfwSetCharCallback(m_Window, char_callback_imgui);
		}
		else
		{
			glfwSetKeyCallback(m_Window, key_callback);
			glfwSetMouseButtonCallback(m_Window, mouse_button_callback);
			glfwSetScrollCallback(m_Window, scroll_callback);
		}

		// Check to see if v-sync was enabled and act accordingly
		if (s_VSync) {
			glfwSwapInterval(1);
		}
		else {
			glfwSwapInterval(0);
		}

		// Initialize GLEW (allows us to use newer versions of OpenGL)
		if (glewInit() != GLEW_OK) {
			ARC_LOG_FATAL("Failed to initialize GLEW");
			return 0;
		}
		ARC_LOG_TRACE("OpenGL {0}", glGetString(GL_VERSION));

		// Setup default OpenGL viewport
		glViewport(0, 0, s_Width, s_Height);

		// Error callback setup
#ifdef ARC_DEV_BUILD
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(DebugMessageCallback, 0);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, GL_FALSE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, nullptr, GL_TRUE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
#endif // ARC_DEV_BUILD
		
		// Everything was successful so return true
		return 1;
	}

	void Window::Update() {
		// Error reporting
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			ARC_LOG_ERROR("OpenGL Error: {0}", error);
		}

		// Input handling
		g_InputManager.Update();

		// Handle Window updating
		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}

	void Window::Clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void Window::Bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	bool Window::Closed() const {
		return glfwWindowShouldClose(m_Window);
	}

	// Sets the Window's Size to the Primary Monitor's Resolution
	void Window::SetFullscreenResolution() {
		const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		s_Width = mode->width;
		s_Height = mode->height;
	}

	/*              Callback Functions              */
	static void error_callback(int error, const char* description) {
		ARC_LOG_ERROR("Error: {0} - {1}", error, description);
	}

	static void window_close_callback(GLFWwindow *window)
	{
		Window* win = (Window*)glfwGetWindowUserPointer(window);

		WindowCloseEvent event;
		win->m_Application->OnEvent(event);
	}

	static void window_resize_callback(GLFWwindow *window, int width, int height) {
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		if (width == 0 || height == 0) {
			win->s_Width = WINDOW_X_RESOLUTION;
			win->s_Height = WINDOW_Y_RESOLUTION;
		}
		else {
			win->s_Width = width;
			win->s_Height = height;
		}
		glViewport(0, 0, win->s_Width, win->s_Height);

		WindowResizeEvent event(static_cast<uint32_t>(win->s_Width), static_cast<uint32_t>(win->s_Height));
		win->m_Application->OnEvent(event);
	}

	static void framebuffer_resize_callback(GLFWwindow *window, int width, int height) {
		Window* win = (Window*)glfwGetWindowUserPointer(window);
	}

#ifdef ARC_DEV_BUILD
	static void UpdateUIState(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		if (key == GLFW_KEY_P && action == GLFW_RELEASE)
		{
			win->s_HideCursor = !win->s_HideCursor;
			GLenum cursorOption = win->s_HideCursor ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
			glfwSetInputMode(win->m_Window, GLFW_CURSOR, cursorOption);
		}
		if (key == GLFW_KEY_U && action == GLFW_RELEASE)
		{
			win->s_HideUI = !win->s_HideUI;
		}
	}
#endif // ARC_DEV_BUILD

	static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
		g_InputManager.KeyCallback(key, scancode, action, mods);

		ARC_DEV_ONLY(UpdateUIState(window, key, scancode, action, mods));
	}

	static void key_callback_imgui(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		g_InputManager.KeyCallback(key, scancode, action, mods);
		ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

		ARC_DEV_ONLY(UpdateUIState(window, key, scancode, action, mods));
	}

	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
		g_InputManager.MouseButtonCallback(button, action, mods);
	}

	static void mouse_button_callback_imgui(GLFWwindow* window, int button, int action, int mods)
	{
		g_InputManager.MouseButtonCallback(button, action, mods);
		ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
	}

	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
		g_InputManager.CursorPositionCallback(xpos, ypos);
	}
	
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		g_InputManager.ScrollCallback(xoffset, yoffset);
	}

	static void scroll_callback_imgui(GLFWwindow* window, double xoffset, double yoffset) {
		g_InputManager.ScrollCallback(xoffset, yoffset);
		ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	}

	static void char_callback_imgui(GLFWwindow* window, unsigned int c) {
		ImGui_ImplGlfw_CharCallback(window, c);
	}

	static void joystick_callback(int joystick, int event) {
		g_InputManager.JoystickCallback(joystick, event);
	}

	static void GLAPIENTRY DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
		ARC_LOG_WARN("GL CALLBACK: type {0} - severity {1} - message {2}", type, severity, message);
	}
}
