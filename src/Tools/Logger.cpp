#include "Logger.h"

std::shared_ptr<Logger> Logger::s_Instance = nullptr;

void Logger::Init()
{
	s_Instance.reset(new Logger());
}

Logger Logger::Get()
{
	return *s_Instance;
}
