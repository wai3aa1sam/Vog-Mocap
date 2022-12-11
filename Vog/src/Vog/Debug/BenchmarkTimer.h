#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include <chrono>
#include <string>

namespace vog {
	class BenchmarkTimer
	{
	public:
		BenchmarkTimer()
		{
			reset();
		}
		BenchmarkTimer(const std::string& name_)
			: 
			m_name(name_)
		{
			reset();
		}

		~BenchmarkTimer()
		{
			float elapsed_time = elapsedMillis();
			std::cout << m_name <<": total time: " << elapsed_time << "ms\n";
		}

		void reset()
		{
			m_start = std::chrono::high_resolution_clock::now();
		}

		float elapsed()
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_start).count() * 0.001f * 0.001f * 0.001f;
		}

		float elapsedMillis()
		{
			return elapsed() * 1000.0f;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
		std::string m_name;
	};
}