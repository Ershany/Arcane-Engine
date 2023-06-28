#include "arcpch.h"
#include "Time.h"

namespace Arcane
{
	Time::Time()
	{
		m_LastFrame = glfwGetTime();
		m_Delta = 0;
	}

	void Time::Update()
	{
		m_Delta = glfwGetTime() - m_LastFrame;
		m_LastFrame = glfwGetTime();
	}
}
