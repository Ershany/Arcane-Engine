#include "arcpch.h"
#include "GLCache.h"

#include <Arcane/Graphics/Shader.h>

namespace Arcane
{
	GLCache::GLCache() : m_ActiveShaderID(0) {
		// Initialize cache values to ensure garbage data doesn't mess with my GL state
		m_DepthTest = false;
		m_StencilTest = false;
		m_Blend = false;
		m_Cull = false;
		m_FaceToCull = GL_BACK;
		m_Multisample = false;
		m_UsesClipPlane = false;
		m_LineSmooth = false;
		SetDepthTest(true);
		SetFaceCull(true);
		SetClipPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		SetLineSmooth(true);
		m_RedMask = GL_TRUE;
		m_GreenMask = GL_TRUE;
		m_BlueMask = GL_TRUE;
		m_AlphaMask = GL_TRUE;
		m_LineThickness = -1.0f;
	}

	GLCache::~GLCache() {

	}

	GLCache* GLCache::GetInstance() {
		static GLCache cache;
		return &cache;
	}

	void GLCache::SetDepthTest(bool choice) {
		if (m_DepthTest != choice) {
			m_DepthTest = choice;
			if (m_DepthTest)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
		}
	}

	void GLCache::SetStencilTest(bool choice) {
		if (m_StencilTest != choice) {
			m_StencilTest = choice;
			if (m_StencilTest)
				glEnable(GL_STENCIL_TEST);
			else
				glDisable(GL_STENCIL_TEST);
		}
	}

	void GLCache::SetBlend(bool choice) {
		if (m_Blend != choice) {
			m_Blend = choice;
			if (m_Blend)
				glEnable(GL_BLEND);
			else
				glDisable(GL_BLEND);
		}
	}

	void GLCache::SetFaceCull(bool choice) {
		if (m_Cull != choice) {
			m_Cull = choice;
			if (m_Cull)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
		}
	}

	void GLCache::SetMultisample(bool choice) {
		if (m_Multisample != choice) {
			m_Multisample = choice;
			if (m_Multisample)
				glEnable(GL_MULTISAMPLE);
			else
				glDisable(GL_MULTISAMPLE);
		}
	}

	void GLCache::SetUsesClipPlane(bool choice)
	{
		if (m_UsesClipPlane != choice)
		{
			m_UsesClipPlane = choice;
			if (m_UsesClipPlane)
				glEnable(GL_CLIP_DISTANCE0);
			else
				glDisable(GL_CLIP_DISTANCE0);
		}
	}

	void GLCache::SetLineSmooth(bool choice)
	{
		if (m_LineSmooth != choice)
		{
			m_LineSmooth = choice;
			if (m_LineSmooth)
				glEnable(GL_LINE_SMOOTH);
			else
				glDisable(GL_LINE_SMOOTH);
		}
	}

	void GLCache::SetDepthFunc(GLenum depthFunc) {
		if (m_DepthFunc != depthFunc) {
			m_DepthFunc = depthFunc;
			glDepthFunc(m_DepthFunc);
		}
	}

	void GLCache::SetStencilFunc(GLenum testFunc, int stencilFragValue, unsigned int stencilBitmask /*=0xFF*/) {
		if (m_StencilTestFunc != testFunc || m_StencilFragValue != stencilFragValue || m_StencilFuncBitmask != stencilBitmask) {
			m_StencilTestFunc = testFunc; 
			m_StencilFragValue = stencilFragValue; 
			m_StencilFuncBitmask = stencilBitmask;

			glStencilFuncSeparate(GL_FRONT_AND_BACK, m_StencilTestFunc, m_StencilFragValue, m_StencilFuncBitmask);
		}
	}

	void GLCache::SetStencilOp(GLenum stencilFailOperation, GLenum depthFailOperation, GLenum depthPassOperation) {
		if (m_StencilFailOperation != stencilFailOperation || m_DepthFailOperation != depthFailOperation || m_DepthPassOperation != depthPassOperation) {
			m_StencilFailOperation = stencilFailOperation;
			m_DepthFailOperation = depthFailOperation;
			m_DepthPassOperation = depthPassOperation;

			glStencilOpSeparate(GL_FRONT_AND_BACK, m_StencilFailOperation, m_DepthFailOperation, m_DepthPassOperation);
		}
	}

	void GLCache::SetStencilWriteMask(unsigned int bitmask) {
		if (m_StencilWriteBitmask != bitmask) {
			m_StencilWriteBitmask = bitmask;
			glStencilMaskSeparate(GL_FRONT_AND_BACK, m_StencilWriteBitmask);
		}
	}

	void GLCache::SetColourMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
	{
		if (m_RedMask != red || m_GreenMask != green || m_BlueMask != blue || m_AlphaMask != alpha)
		{
			m_RedMask = red;
			m_GreenMask = green;
			m_BlueMask = blue;
			m_AlphaMask = alpha;
			glColorMask(m_RedMask, m_GreenMask, m_BlueMask, m_AlphaMask);
		}
	}

	void GLCache::SetBlendFunc(GLenum src, GLenum dst) {
		if (m_BlendSrc != src || m_BlendDst != dst) {
			m_BlendSrc = src;
			m_BlendDst = dst;
			glBlendFunc(m_BlendSrc, m_BlendDst);
		}
	}

	void GLCache::SetCullFace(GLenum faceToCull) {
		if (m_FaceToCull != faceToCull) {
			m_FaceToCull = faceToCull;
			glCullFace(m_FaceToCull);
		}
	}

	void GLCache::SetClipPlane(glm::vec4 clipPlane)
	{
		m_ActiveClipPlane = clipPlane;
	}

	void GLCache::SetLineWidth(float lineThickness)
	{
		if (m_LineThickness != lineThickness)
		{
			m_LineThickness = lineThickness;
			glLineWidth(m_LineThickness);
		}
	}

	void GLCache::SetShader(Shader *shader) {
		if (m_ActiveShaderID != shader->GetShaderID()) {
			m_ActiveShaderID = shader->GetShaderID();
			shader->Enable();
		}
	}

	void GLCache::SetShader(unsigned int shaderID) {
		if (m_ActiveShaderID != shaderID) {
			m_ActiveShaderID = shaderID;
			glUseProgram(shaderID);
		}
	}
}
