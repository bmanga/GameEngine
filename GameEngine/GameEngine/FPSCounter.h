#pragma once
#include "Timer.h"
namespace Lemur{
namespace utils{
	
class FPSCounter
{
public:
	void start()
	{
		m_timer.start();
	}

	void tick()
	{
		++m_counter;
	}

	void update()
	{
		auto elapsed = m_timer.elapsed();
		m_fps = m_counter / elapsed;
		m_counter = 0;
		m_timer.start();
	}

	float fps() const
	{
		return m_fps;
	}

private:
	RunningTimer m_timer;
	int m_counter = 0;
	float m_fps = 0;
};

}// namespace utils
}// namespace Lemur