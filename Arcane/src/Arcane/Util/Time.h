#pragma once

namespace Arcane
{
	struct Time {
	private:
		GLdouble m_LastFrame;
		GLdouble m_Delta;
	public:
		Time();

		void Update();

		// Getters
		inline GLdouble GetDeltaTime() const { return m_Delta; }
	};
}
