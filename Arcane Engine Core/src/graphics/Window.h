#pragma once

#include <input/InputManager.h>
#include <vendor/imgui/imgui.h>
#include <vendor/imgui/imgui_impl_glfw_gl3.h>

namespace arcane {

	static InputManager g_InputManager;

	class Window {
	public:
		Window(const char *title, int width, int height);
		~Window();

		/**
		* Will swap the screen buffers and will poll all window/input events
		*/
		void update();
		bool closed() const;
		static void clear();
		static void bind();

		static inline bool getHideCursor() { return s_HideCursor; }
		static inline int getWidth() { return s_Width; }
		static inline int getHeight() { return s_Height; }
		static inline int getResolutionWidth() { return (int)(s_Width * SUPERSAMPLING_FACTOR); }
		static inline int getResolutionHeight() { return (int)(s_Height * SUPERSAMPLING_FACTOR); }
	private:
		bool init();

		void setFullscreenResolution();

		// Callback Functions
		static friend void error_callback(int error, const char* description);
		static friend void window_resize_callback(GLFWwindow *window, int width, int height);
		static friend void framebuffer_resize_callback(GLFWwindow *window, int width, int height);
		static friend void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
		static friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		static friend void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
		static friend void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static friend void char_callback(GLFWwindow* window, unsigned int c);
		static friend void joystick_callback(int joystick, int event);
		static friend void GLAPIENTRY DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
	private:
		const char *m_Title;
		GLFWwindow *m_Window;

		static bool s_HideCursor;
		static int s_Width, s_Height;
	};

}
