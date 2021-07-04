#pragma once

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
