#pragma once
#ifndef TIMER_H
#define TIMER_H

namespace Arcane
{
	class Timer {
	public:
		Timer();

		void Reset();
		void Rewind(double time);

		double Elapsed() const;
	private:
		double m_StartTime;
	};
}
#endif
