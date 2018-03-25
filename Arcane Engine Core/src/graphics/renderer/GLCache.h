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
		void setStencilTest(bool choice);
		void setBlend(bool choice);
		void setCull(bool choice);

		void setDepthFunc(GLenum depthFunc);
		void setStencilFunc(GLenum testFunc, GLint stencilFragValue, GLuint stencilBitmask);
		void setStencilOp(GLenum stencilFailOperation, GLenum depthFailOperation, GLenum depthPassOperation);
		void setStencilWriteMask(GLuint bitmask);
		void setBlendFunc(GLenum src, GLenum dst);
		void setCullFace(GLenum faceToCull);

		void switchShader(GLuint shaderID);
	private:
		// Toggles
		bool m_DepthTest;
		bool m_StencilTest;
		bool m_Blend;
		bool m_Cull;

		// Depth State
		GLenum m_DepthFunc;

		// Stencil State
		GLenum m_StencilTestFunc;
		GLint m_StencilFragValue;
		GLuint m_StencilFuncBitmask;

		GLenum m_StencilFailOperation, m_DepthFailOperation, m_DepthPassOperation;
		GLuint m_StencilWriteBitmask;

		// Blend State
		GLenum m_BlendSrc, m_BlendDst;

		// Culling State
		GLenum m_FaceToCull;

		// Active binds
		GLuint m_ActiveShaderID;
	};

} }