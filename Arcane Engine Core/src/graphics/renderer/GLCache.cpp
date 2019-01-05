#include "pch.h"
#include "GLCache.h"

namespace arcane {

	GLCache::GLCache() : m_ActiveShaderID(0) {
		// Initialize cache values to ensure garbage data doesn't mess with my GL state
		m_DepthTest = false;
		m_StencilTest = false;
		m_Blend = false;
		m_Cull = false;
		m_FaceToCull = GL_BACK;
		m_Multisample = false;;
		setDepthTest(true);
		setFaceCull(true);
	}

	GLCache::~GLCache() {

	}

	GLCache* GLCache::getInstance() {
		static GLCache cache;
		return &cache;
	}

	void GLCache::setDepthTest(bool choice) {
		if (m_DepthTest != choice) {
			m_DepthTest = choice;
			if (m_DepthTest)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
		}
	}

	void GLCache::setStencilTest(bool choice) {
		if (m_StencilTest != choice) {
			m_StencilTest = choice;
			if (m_StencilTest)
				glEnable(GL_STENCIL_TEST);
			else
				glDisable(GL_STENCIL_TEST);
		}
	}

	void GLCache::setBlend(bool choice) {
		if (m_Blend != choice) {
			m_Blend = choice;
			if (m_Blend)
				glEnable(GL_BLEND);
			else
				glDisable(GL_BLEND);
		}
	}

	void GLCache::setFaceCull(bool choice) {
		if (m_Cull != choice) {
			m_Cull = choice;
			if (m_Cull)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
		}
	}

	void GLCache::setMultisample(bool choice) {
		if (m_Multisample != choice) {
			m_Multisample = choice;
			if (m_Multisample)
				glEnable(GL_MULTISAMPLE);
			else
				glDisable(GL_MULTISAMPLE);
		}
	}

	void GLCache::setDepthFunc(GLenum depthFunc) {
		if (m_DepthFunc != depthFunc) {
			m_DepthFunc = depthFunc;
			glDepthFunc(m_DepthFunc);
		}
	}

	void GLCache::setStencilFunc(GLenum testFunc, int stencilFragValue, unsigned int stencilBitmask) {
		if (m_StencilTestFunc != testFunc || m_StencilFragValue != stencilFragValue || m_StencilFuncBitmask != stencilBitmask) {
			m_StencilTestFunc = testFunc; 
			m_StencilFragValue = stencilFragValue; 
			m_StencilFuncBitmask = stencilBitmask;

			glStencilFuncSeparate(GL_FRONT_AND_BACK, m_StencilTestFunc, m_StencilFragValue, m_StencilFuncBitmask);
		}
	}

	void GLCache::setStencilOp(GLenum stencilFailOperation, GLenum depthFailOperation, GLenum depthPassOperation) {
		if (m_StencilFailOperation != stencilFailOperation || m_DepthFailOperation != depthFailOperation || m_DepthPassOperation != depthPassOperation) {
			m_StencilFailOperation = stencilFailOperation;
			m_DepthFailOperation = depthFailOperation;
			m_DepthPassOperation = depthPassOperation;

			glStencilOpSeparate(GL_FRONT_AND_BACK, m_StencilFailOperation, m_DepthFailOperation, m_DepthPassOperation);
		}
	}

	void GLCache::setStencilWriteMask(unsigned int bitmask) {
		if (m_StencilWriteBitmask != bitmask) {
			m_StencilWriteBitmask = bitmask;
			glStencilMaskSeparate(GL_FRONT_AND_BACK, m_StencilWriteBitmask);
		}
	}

	void GLCache::setBlendFunc(GLenum src, GLenum dst) {
		if (m_BlendSrc != src || m_BlendDst != dst) {
			m_BlendSrc = src;
			m_BlendDst = dst;
			glBlendFunc(m_BlendSrc, m_BlendDst);
		}
	}

	void GLCache::setCullFace(GLenum faceToCull) {
		if (m_FaceToCull != faceToCull) {
			m_FaceToCull = faceToCull;
			glCullFace(m_FaceToCull);
		}
	}

	void GLCache::switchShader(Shader *shader) {
		if (m_ActiveShaderID != shader->getShaderID()) {
			m_ActiveShaderID = shader->getShaderID();
			shader->enable();
		}
	}

	void GLCache::switchShader(unsigned int shaderID) {
		if (m_ActiveShaderID != shaderID) {
			m_ActiveShaderID = shaderID;
			glUseProgram(shaderID);
		}
	}

}
