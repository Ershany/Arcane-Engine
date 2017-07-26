#define V_SYNC 1
#define FULLSCREEN_MODE 1
#define SHOW_MOUSE 0

#include "Window.h"

namespace arcane { namespace graphics {

	Window::Window(const char *title, int width, int height) {
		m_Title = title;
		m_Width = width;
		m_Height = height;

		if (!init()) {
			utils::Logger::getInstance().error("logged_files/window_creation.txt", "Window Initialization", "Could not initialize window class");
			glfwDestroyWindow(m_Window);
			glfwTerminate();
		}
		
		memset(m_Keys, 0, sizeof(bool) * MAX_KEYS);
		memset(m_Buttons, 0, sizeof(bool) * MAX_BUTTONS);
	}

	Window::~Window() {
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	bool Window::init() {
		if (!glfwInit()) {
			std::cout << "GLFW Failed To Initialize" << std::endl;
			utils::Logger::getInstance().error("logged_files/window_creation.txt", "Window Initialization", "Could not initialize the GLFW window");
		}
		

		// Create the window
		if (FULLSCREEN_MODE) {
			setFullscreenResolution();
			m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, glfwGetPrimaryMonitor(), NULL);
		}
		else {
			m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, NULL, NULL);
		}
		
		if (!m_Window) {
			utils::Logger::getInstance().error("logged_files/window_creation.txt", "Window Initialization", "Could not create the GLFW window");
			std::cout << "GLFW Window Couldn't Be Created" << std::endl;
		}

		// Setup the mouse settings
		if (!SHOW_MOUSE) 
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// Set up contexts and callbacks
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, this);
		glfwSetErrorCallback(error_callback);
		glfwSetWindowSizeCallback(m_Window, window_resize);
		glfwSetKeyCallback(m_Window, key_callback);
		glfwSetMouseButtonCallback(m_Window, mouse_button_callback);
		glfwSetCursorPosCallback(m_Window, cursor_position_callback);
		glfwSetScrollCallback(m_Window, scroll_callback);


		// Check to see if v-sync was enabled and act accordingly
		if (V_SYNC) {
			glfwSwapInterval(1);
		}
		else {
			glfwSwapInterval(0);
		}
		

		// Initialize GLEW (allows us to use newer versions of OpenGL)
		if (glewInit() != GLEW_OK) {
			std::cout << "Could not Initialize GLEW" << std::endl;
			utils::Logger::getInstance().error("logged_files/window_creation.txt", "Window Initialization", "Could not initialize the GLEW");
			return 0;
		}
		std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
		
		// Everything was successful so return true
		return 1;
	}

	void Window::update() {
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "OpenGL Error: " << error << std::endl;
		}

		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}

	void Window::clear() const {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	bool Window::closed() const {
		return glfwWindowShouldClose(m_Window);
	}

	// Sets the Window's Size to the Primary Monitor's Resolution
	void Window::setFullscreenResolution() {
		const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		m_Width = mode->width;
		m_Height = mode->height;
	}

	/*                   Getters                    */
	bool Window::isKeyPressed(unsigned int keycode) const {
		if (keycode >= MAX_KEYS) {
			//TODO: LOG THIS
			std::cout << "Max key overflow in window" << std::endl;
			return false;
		}
		else {
			return m_Keys[keycode];
		}
	}

	bool Window::isMouseButtonPressed(unsigned int code) const {
		if (code >= MAX_BUTTONS) {
			//TODO: LOG THIS
			std::cout << "Max mouse button overflow in window" << std::endl;
			return false;
		}
		else {
			return m_Buttons[code];
		}
	}


	/*              Callback Functions              */
	static void error_callback(int error, const char* description) {
		std::cout << "Error:" << std::endl << description << std::endl;
	}

	static void window_resize(GLFWwindow *window, int width, int height) {
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		win->m_Width = width;
		win->m_Height = height;
		glViewport(0, 0, win->m_Width, win->m_Height);
	}

	static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		win->m_Keys[key] = action != GLFW_RELEASE;
	}

	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		win->m_Buttons[button] = action != GLFW_RELEASE;
	}

	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		win->mx = xpos;
		win->my = ypos;
	}
	
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		win->scrollX = xoffset;
		win->scrollY = yoffset;
	}

} }