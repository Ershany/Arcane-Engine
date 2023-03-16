#pragma once

#include <Arcane/Util/Singleton.h>

namespace Arcane
{
	class Shader;

	class GLCache : Singleton {
	public:
		GLCache();
		~GLCache();

		static GLCache* GetInstance();

		void SetDepthTest(bool choice);
		void SetStencilTest(bool choice);
		void SetBlend(bool choice);
		void SetFaceCull(bool choice);
		void SetMultisample(bool choice);
		void SetUsesClipPlane(bool choice);

		void SetDepthFunc(GLenum depthFunc);
		void SetStencilFunc(GLenum testFunc, int stencilFragValue, unsigned int stencilBitmask = 0xFF);
		void SetStencilOp(GLenum stencilFailOperation, GLenum depthFailOperation, GLenum depthPassOperation);
		void SetStencilWriteMask(unsigned int bitmask);
		void SetColourMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
		void SetBlendFunc(GLenum src, GLenum dst);
		void SetCullFace(GLenum faceToCull);
		void SetClipPlane(glm::vec4 clipPlane);

		void SetShader(Shader *shader);
		void SetShader(unsigned int shaderID);

		inline bool GetUsesClipPlane() { return m_UsesClipPlane; }
		inline const glm::vec4& GetActiveClipPlane() { return m_ActiveClipPlane; }
	private:
		// Toggles
		bool m_DepthTest;
		bool m_StencilTest;
		bool m_Blend;
		bool m_Cull;
		bool m_Multisample;
		bool m_UsesClipPlane;

		// Depth State
		GLenum m_DepthFunc;

		// Stencil State
		GLenum m_StencilTestFunc;
		int m_StencilFragValue;
		unsigned int m_StencilFuncBitmask;

		GLenum m_StencilFailOperation, m_DepthFailOperation, m_DepthPassOperation;
		unsigned int m_StencilWriteBitmask;

		// Colour State
		GLboolean m_RedMask, m_BlueMask, m_GreenMask, m_AlphaMask;

		// Blend State
		GLenum m_BlendSrc, m_BlendDst;

		// Culling State
		GLenum m_FaceToCull;

		// Clip Plane State
		glm::vec4 m_ActiveClipPlane;

		// Active binds
		unsigned int m_ActiveShaderID;
	};
}
