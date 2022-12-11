#include "vogpch.h"
#include "Vog/Core/Timer.h"

#include <Windows.h>
#include <Windowsx.h>

// Reference
// https://github.com/SimpleTalkCpp/SimpleTalkCpp_Tutorial/blob/main/Graphic/Advance%20OpenGL/Advance%20OpenGL%20003/my_common.h

namespace vog 
{
#pragma region WindowTimer_Class
	class WindowsTimer
	{
	public:
		WindowsTimer()
		{
			LARGE_INTEGER f;
			QueryPerformanceFrequency(&f);
			m_freq = my_to_uint64(f);
			reset();
		}
		~WindowsTimer() = default;

		double get()
		{
			auto c = _getTick();
			return (double)(c - m_start) / m_freq;
		}
		void reset()
		{
			m_start = _getTick();
		}

	private:
		uint64_t _getTick()
		{
			LARGE_INTEGER v;
			QueryPerformanceCounter(&v);
			return my_to_uint64(v);
		}
	private:
		uint64_t m_freq;
		uint64_t m_start;
	};
#pragma endregion

	static WindowsTimer s_timer = WindowsTimer();

	float Timer::getTime()
	{
		return static_cast<float>(s_timer.get());
	}

	float Timer::getMilliTime()
	{
		return getTime() * 0.001f;
	}

	void Timer::reset()
	{
		s_timer.reset();
	}

}