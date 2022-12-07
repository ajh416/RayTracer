#pragma once

#include "Timer.h"

#include <fstream>

class InstrumentationTimer
{
	friend class Instrumentor;

	struct InstrumentationResult
	{
		std::string_view Name;
		int64_t Start;
		int64_t End;
	};
public:
	InstrumentationTimer(const std::string& name) : m_Name(name)
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	~InstrumentationTimer() { if (!m_Stopped) Stop(); }

	void Stop()
	{
		auto stopTimepoint = std::chrono::high_resolution_clock::now();
		auto start = std::chrono::time_point_cast<std::chrono::milliseconds>(m_StartTimepoint).time_since_epoch().count();
		auto end = std::chrono::time_point_cast<std::chrono::milliseconds>(stopTimepoint).time_since_epoch().count();

		Logger::Get().Debug("Timer {0} took {1} milliseconds to complete!", m_Name, end - start);

		m_Stopped = true;
	}

private:
	std::string m_Name;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
	bool m_Stopped = false;
};

class Instrumentor
{

};