#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>

namespace arcane { namespace graphics {

#define MAX_KEYS 1024
#define MAX_BUTTONS 32

	class Window {
	private:
		const char *m_Title;
		int m_Width, m_Height;
		GLFWwindow *m_Window;
		
		bool m_Keys[MAX_KEYS];
		bool m_Buttons[MAX_BUTTONS];
		double mx, my;
		double scrollX, scrollY;
	public:
		Window(const char *title, int width, int height);
		~Window();
		void update();
		void clear() const;
		bool closed() const;
		bool isKeyPressed(unsigned int keycode) const;
		bool isMouseButtonPressed(unsigned int keycode) const;

		inline double getMouseX() { return mx; }
		inline double getMouseY() { return my; }
		inline double getScrollX() { return scrollX; }
		inline double getScrollY() { return scrollY; }
		inline void resetScroll() { scrollX = 0; scrollY = 0; }
		inline void getMousePosition(double &x, double &y) { x = mx; y = my; }
		inline int getWidth() { return m_Width; }
		inline int getHeight() { return m_Height; }
	private:
		bool init();
		void setResolution();
		static friend void error_callback(int error, const char* description);
		static friend void window_resize(GLFWwindow *window, int width, int height);
		static friend void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
		static friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		static friend void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
		static friend void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	};

} }