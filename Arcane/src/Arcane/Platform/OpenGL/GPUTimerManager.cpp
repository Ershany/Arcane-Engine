#include "arcpch.h"
#include "GPUTimerManager.h"

#include <Arcane/Vendor/Imgui/imgui.h>

namespace Arcane
{
#ifdef ARC_DEV_BUILD
	/*								GPU Timer								*/
	GPUTimer::GPUTimer(const std::string &timerName) : m_TimerName(timerName), m_IsUsed(false)
	{
		m_ElapsedTimeNanoseconds[0] = 0;
		m_ElapsedTimeNanoseconds[1] = 0;

		glGenQueries(2, m_Query);
	}

	GPUTimer::~GPUTimer()
	{
		glDeleteQueries(2, m_Query);
	}

	void GPUTimer::BeginQuery(bool oddFrame)
	{
		m_IsUsed = true;
		glBeginQuery(GL_TIME_ELAPSED, m_Query[oddFrame]);
	}

	void GPUTimer::EndQuery()
	{
		glEndQuery(GL_TIME_ELAPSED);
	}

	void GPUTimer::CalculateTime(bool oddFrame)
	{
		// Note: This function will block and wait until the work is done, that is why this gets called for the opposite frame, so we are getting the previous frames timing and not stalling. This is all handled by the GPUTimerManager
		if (!m_IsUsed)
			return;

		glGetQueryObjectui64v(m_Query[!oddFrame], GL_QUERY_RESULT, &m_ElapsedTimeNanoseconds[!oddFrame]);
	}



	/*								GPU Timer Manager								*/
	bool GPUTimerManager::s_OddFrame = false;
	bool GPUTimerManager::s_FirstFrame = true;
	std::vector<GPUTimer*> GPUTimerManager::s_Timers;

	void GPUTimerManager::Startup()
	{
		s_Timers.reserve(20);
	}

	void GPUTimerManager::Shutdown()
	{
		s_Timers.clear();
	}

	void GPUTimerManager::EndOfFrameUpdate()
	{
		s_OddFrame = !s_OddFrame;
		s_FirstFrame = false;
	}

	GPUTimer* GPUTimerManager::CreateGPUTimer(const std::string &timerName)
	{
		s_Timers.push_back(new GPUTimer(timerName));
		return s_Timers[s_Timers.size() - 1];
	}

	void GPUTimerManager::BeginQuery(GPUTimer *timer)
	{
		timer->BeginQuery(s_OddFrame);
	}

	void GPUTimerManager::EndQuery(GPUTimer *timer)
	{
		timer->EndQuery();
	}

	void GPUTimerManager::BuildImguiTimerUI()
	{
		// Need to use an anti-pattern, because the query hasn't been started, and OpenGL will spit out a GL_INVALID_OPERATION because no queries that match the ID have been started
		if (s_FirstFrame)
		{
			return;
		}

		for (auto& element : s_Timers)
		{
			element->CalculateTime(s_OddFrame);
			double ms = element->GetTimeMS(s_OddFrame);

			ImGui::Text("%s - %f", element->m_TimerName.c_str(), ms);
		}
	}
#endif
}
