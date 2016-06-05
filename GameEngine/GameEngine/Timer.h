#pragma once
#include <chrono>

#include "ConsoleLogger.h"
#include "Lemur.h"

namespace Lemur {
namespace utils {
class Timer
{
public:
	void start() noexcept
	{
		m_start = std::chrono::high_resolution_clock::now();
		running = true;
	}
	void stop() noexcept
	{
		m_end = std::chrono::high_resolution_clock::now();
		running = false;
	}
	void reset() noexcept
	{
		m_start = std::chrono::high_resolution_clock::now();
	}
	auto elapsed() noexcept
	{
		namespace chr = std::chrono;
		if (running)
			m_end = chr::high_resolution_clock::now();
		return chr::duration_cast<chr::seconds>(m_end - m_start).count();
	}

	auto elapsed_us() noexcept
	{
		namespace chr = std::chrono;
		if (running)
			m_end = m_end = chr::high_resolution_clock::now();
		return chr::duration_cast<chr::microseconds>(m_end - m_start).count();
	}

	auto elapsed_ms() noexcept
	{
		namespace chr = std::chrono;
		if (running)
			m_end = m_end = chr::high_resolution_clock::now();
		return chr::duration_cast<chr::milliseconds>(m_end - m_start).count();
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_end;
	bool running = false;

};

class RunningTimer
{
public:
	void start() noexcept
	{
		m_time = std::chrono::high_resolution_clock::now();
	}

	float elapsed() const noexcept
	{
		namespace chr = std::chrono;
		auto now = std::chrono::high_resolution_clock::now();
		return chr::duration_cast<chr::microseconds>(now - m_time)
			.count() / float(1e6);
	}

	auto elapsed_ms() const noexcept
	{
		namespace chr = std::chrono;
		auto now = std::chrono::high_resolution_clock::now();
		return chr::duration_cast<chr::milliseconds>(now - m_time).count();
	}

	auto elapsed_us() const noexcept
	{
		namespace chr = std::chrono;
		auto now = std::chrono::high_resolution_clock::now();
		return chr::duration_cast<chr::microseconds>(now - m_time).count();
	}

	void reset() noexcept
	{
		m_time = std::chrono::high_resolution_clock::now();
	}
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_time;
};

template <class Fn>
float measure_time(Fn&& fn)
{
	RunningTimer t;
	t.start();
	std::forward<Fn>(fn) ();
	return t.elapsed();
}

inline void LogMeasuredTime(const char* codeloc, float seconds) noexcept 
{
	Lemur::ConsoleLogger::Info(codeloc, cstr("elapsed time: ", seconds, " s"));
}

class ScopeTimeLogger
{
public:
	ScopeTimeLogger(const char* source) noexcept : m_source(source)
	{
		m_timer.start();
	}
	~ScopeTimeLogger() noexcept
	{
		LogMeasuredTime(m_source.c_str(), m_timer.elapsed());
	}
	
private:
	RunningTimer m_timer;
	std::string m_source;
};

}// namespace utils
}// namespace Lemur


//some utility macros

#define LOG_MEASURE_TIME_BEGIN \
Lemur::utils::LogMeasuredTime(CODE_LOCATION, Lemur::utils::measure_time([&](){

#define LOG_MEASURE_TIME_END \
}));

#ifndef NDEBUG
#define DEB_LOG_MEASURE_TIME_BEGIN LOG_MEASURE_TIME_BEGIN
#define DEB_LOG_MEASURE_TIME_END   LOG_MEASURE_TIME_END
#define DEB_LOG_SCOPE_TIME \
Lemur::utils::ScopeTimeLogger __scopedTimeLogger(CODE_LOCATION);
#else
#define DEBUG_LOG_MEASURE_TIME_BEGIN 
#define DEBUG_LOG_MEASURE_TIME_END   
#define DEB_LOG_SCOPE_TIME
#endif
