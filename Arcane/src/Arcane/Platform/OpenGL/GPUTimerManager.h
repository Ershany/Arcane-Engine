#pragma once

namespace Arcane
{
#ifdef ARC_DEV_BUILD
	// This class stores queries for even and odd frames. This is because when we query for data OpenGL will stall. So what we do for the current frame is start and stop the timers
	// and we do not sample these times until the next frame. This should avoid any stalling waiting for a timer
	class GPUTimer
	{
		friend class GPUTimerManager;
	public:
		~GPUTimer();
	private:
		GPUTimer(const std::string &timerName);

		void BeginQuery(bool oddFrame);
		void EndQuery();
		void CalculateTime(bool oddFrame);

		inline double GetTimeMS(bool oddFrame) const { return double(m_ElapsedTimeNanoseconds[!oddFrame]) / 1000000.0; }
	private:
		const std::string m_TimerName;
		GLuint m_Query[2];
		GLuint64 m_ElapsedTimeNanoseconds[2];
		bool m_IsUsed;
	};

	class GPUTimerManager
	{
	public:
		static void Startup();
		static void Shutdown();

		static void EndOfFrameUpdate();

		// Can be used by classes to create a timer and setup the start and end of the query
		static GPUTimer* CreateGPUTimer(const std::string &timerName);
		static void BeginQuery(GPUTimer *timer);
		static void EndQuery(GPUTimer *timer);

		// Used to get the time of every timer and the name
		static void BuildImguiTimerUI();
	private:
		static bool s_OddFrame; 
		static bool s_FirstFrame; // Eww Anti-pattern
		static std::vector<GPUTimer*> s_Timers;
	};
#endif
}
