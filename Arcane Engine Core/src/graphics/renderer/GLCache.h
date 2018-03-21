#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "../../utils/Singleton.h"

#include <iostream>

namespace arcane { namespace graphics {

	class GLCache : utils::Singleton {
	public:
		GLCache();
		~GLCache();

		static GLCache* getInstance();

		void setDepthTest(bool choice);
		void setBlend(bool choice);
		void setCull(bool choice);

		void setBlendFunc(GLenum src, GLenum dst);
		void setDepthFunc(GLenum depthFunc);
		void setCullFace(GLenum faceToCull);

		void switchShader(GLuint shaderID);
	private:
		// Toggles
		bool m_DepthTest;
		bool m_Blend;
		bool m_Cull;

		// State
		GLenum m_BlendSrc;
		GLenum m_BlendDst;
		GLenum m_DepthFunc;
		GLenum m_FaceToCull;

		// Active binds
		GLuint m_ActiveShaderID;
	};

} }