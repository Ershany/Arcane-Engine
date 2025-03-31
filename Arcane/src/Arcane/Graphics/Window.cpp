#include "arcpch.h"
#include "Window.h"

#include <Arcane/Vendor/Imgui/examples/imgui_impl_glfw.h>
#include <Arcane/Input/InputManager.h>

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

		// Error callback setup
#if USE_OPENGL_DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

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
		InputManager::GetInstance().SetMousePos(currMouseX, currMouseY);

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

		// More error callback setup
#if USE_OPENGL_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, 0);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, GL_FALSE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, nullptr, GL_TRUE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
#endif

#if USE_RENDERDOC
		if (GLEW_KHR_debug)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		}
#endif
		
		// Everything was successful so return true
		return 1;
	}

	void Window::Update() {
		// Error reporting
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			ARC_LOG_ERROR("OpenGL Error: {0}", error);
		}

		// Handle Window updating
		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}

	void Window::ClearAll() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void Window::ClearColour()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Window::ClearDepth()
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void Window::ClearStencil()
	{
		glClear(GL_STENCIL_BUFFER_BIT);
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
		InputManager::GetInstance().KeyCallback(key, scancode, action, mods);

		ARC_DEV_ONLY(UpdateUIState(window, key, scancode, action, mods));
	}

	static void key_callback_imgui(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		InputManager::GetInstance().KeyCallback(key, scancode, action, mods);
		ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

		ARC_DEV_ONLY(UpdateUIState(window, key, scancode, action, mods));
	}

	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
		InputManager::GetInstance().MouseButtonCallback(button, action, mods);
	}

	static void mouse_button_callback_imgui(GLFWwindow* window, int button, int action, int mods)
	{
		InputManager::GetInstance().MouseButtonCallback(button, action, mods);
		ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
	}

	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
		InputManager::GetInstance().CursorPositionCallback(xpos, ypos);
	}
	
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		InputManager::GetInstance().ScrollCallback(xoffset, yoffset);
	}

	static void scroll_callback_imgui(GLFWwindow* window, double xoffset, double yoffset) {
		InputManager::GetInstance().ScrollCallback(xoffset, yoffset);
		ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	}

	static void char_callback_imgui(GLFWwindow* window, unsigned int c) {
		ImGui_ImplGlfw_CharCallback(window, c);
	}

	static void joystick_callback(int joystick, int event) {
		InputManager::GetInstance().JoystickCallback(joystick, event);
	}

	void APIENTRY glDebugOutput(GLenum source,
		GLenum type,
		unsigned int id,
		GLenum severity,
		GLsizei length,
		const char* message,
		const void* userParam)
	{
		// ignore non-significant error/warning codes
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

		ARC_LOG_WARN("---------------");
		ARC_LOG_WARN("Debug message ({0}) {1}", id, message);

		switch (source)
		{
		case GL_DEBUG_SOURCE_API:             ARC_LOG_WARN("Source: API"); break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   ARC_LOG_WARN("Source: Window System"); break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: ARC_LOG_WARN("Source: Shader Compiler"); break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     ARC_LOG_WARN("Source: Third Party"); break;
		case GL_DEBUG_SOURCE_APPLICATION:     ARC_LOG_WARN("Source: Application"); break;
		case GL_DEBUG_SOURCE_OTHER:           ARC_LOG_WARN("Source: Other"); break;
		}

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:               ARC_LOG_WARN("Type: Error"); break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: ARC_LOG_WARN("Type: Deprecated Behaviour"); break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  ARC_LOG_WARN("Type: Undefined Behaviour"); break;
		case GL_DEBUG_TYPE_PORTABILITY:         ARC_LOG_WARN("Type: Portability"); break;
		case GL_DEBUG_TYPE_PERFORMANCE:         ARC_LOG_WARN("Type: Performance"); break;
		case GL_DEBUG_TYPE_MARKER:              ARC_LOG_WARN("Type: Marker"); break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          ARC_LOG_WARN("Type: Push Group"); break;
		case GL_DEBUG_TYPE_POP_GROUP:           ARC_LOG_WARN("Type: Pop Group"); break;
		case GL_DEBUG_TYPE_OTHER:               ARC_LOG_WARN("Type: Other"); break;
		}

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         ARC_LOG_WARN("Severity: high"); break;
		case GL_DEBUG_SEVERITY_MEDIUM:       ARC_LOG_WARN("Severity: medium"); break;
		case GL_DEBUG_SEVERITY_LOW:          ARC_LOG_WARN("Severity: low"); break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: ARC_LOG_WARN("Severity: notification"); break;
		}
	}
}
