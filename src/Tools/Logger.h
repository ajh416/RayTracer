#pragma once

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

#include <memory>
#include <iostream>
#include <string>
#include <typeinfo>
#include <vector>
#include <sstream>

class Logger
{
private:
	enum class LogLevel
	{
		Trace = 0, Info, Warn, Error, Critical
	};

private:
	Logger() = default;

	template<typename... Args>
	void Log(LogLevel lvl, Args&&... args);

public:
	static void Init();

	static Logger Get();

	template<typename... Args>
	void Debug(Args&&... item);

	template<typename... Args>
	void Info(Args&&... item);

	template<typename... Args>
	void Warn(Args&&... item);

	template<typename... Args>
	void Error(Args&&... item);

	template<typename... Args>
	void Critical(Args&&... item);

private:
	static std::shared_ptr<Logger> s_Instance;

	template<typename T, typename... Ts>
	constexpr bool contains()
	{
		return std::disjunction_v<std::is_same<T, typename std::decay<Ts>::type>...>;
	}

	template<class T>
	struct is_c_str
		: std::integral_constant<
		bool,
		std::is_same<char const*, typename std::decay<T>::type>::value ||
		std::is_same<char*, typename std::decay<T>::type>::value
		> {};

	template<class T>
	struct is_str
		: std::integral_constant<
		bool,
		std::is_same<std::string, typename std::decay<T>::type>::value
		> {};

	template<typename... Args>
	inline std::string IsText(Args&&... text)
	{
		std::stringstream ss;

#pragma warning(push)
#pragma warning(disable: 4244)
		([&](auto input)
			{
				if (is_c_str<decltype(input)>::value || is_str<decltype(input)>::value && ss.str().empty())
				{
					if (ss.str() == "")
						ss << input;
				}
			} (text), ...);
#pragma warning(pop)

		return ss.str();
	}

	template<typename... Args>
	constexpr inline std::vector<std::string> ArgToString(Args&&... args)
	{
		std::vector<std::string> argString;

		([&](auto& input)
			{
				std::stringstream ss;
				ss << input;
				argString.push_back(ss.str());
			} (args), ...);

		return argString;
	}

	inline std::string EmbedArgs(const std::string& text, const std::vector<std::string>& vec)
	{
		std::stringstream ss;

		int64_t pos = text.find('{');
		int64_t pos2 = text.substr(pos + 1).find('{');

		if (pos != std::string::npos && pos2 != std::string::npos && vec.size() > 2)
		{
			std::string first = text.substr(0, pos);
			std::string second = text.substr(pos + 3, pos2 - 2);
			std::string last = text.substr(pos + pos2 + 4);

			if (vec.size() > 2)
				ss << first << vec[1] << second << vec[2] << last;
			else
				ss << first << vec[1] << second << " " << last;
		}
		else if (pos != std::string::npos && pos2 == std::string::npos && vec.size() > 1)
		{
			std::string first = text.substr(0, pos);
			std::string last = text.substr(pos + 3);

			ss << first << vec[1] << last;
		}
		else
		{
			for (auto& elem : vec)
			{
				ss << elem << " ";
			}
		}

		return ss.str();
	}
};

template<typename... Args>
inline void Logger::Log(LogLevel lvl, Args&&... args)
{
	std::string text = IsText(std::forward<Args>(args)...);
	std::vector<std::string> stringArgs = ArgToString(std::forward<Args>(args)...);

	std::string color;
	std::string endColor;

	switch (lvl)
	{
	case LogLevel::Trace:
		break;
	case LogLevel::Info:
		color = "\x1B[32m";
		endColor = "\033[0m";
		break;
	case LogLevel::Warn:
		color = "\x1B[93m";
		endColor = "\033[0m";
		break;
	case LogLevel::Error:
		color = "\x1B[31m";
		endColor = "\033[0m";
		break;
	case LogLevel::Critical:
		color = "\u001b[41;1m";
		endColor = "\033[0m";
		break;
	}

	std::string embedText = "";
	embedText = EmbedArgs(text, stringArgs);
	embedText = color + embedText + endColor + "\n";

#ifdef _WIN32
	HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteConsoleA(stdOut, embedText.c_str(), (DWORD)embedText.size(), nullptr, nullptr);
#else
	std::cout << embedText << '\n';
#endif
}

template<typename... Args>
inline void Logger::Debug(Args&&... item)
{
	Log(LogLevel::Trace, std::forward<Args>(item)...);
}

template<typename... Args>
inline void Logger::Info(Args&&... item)
{
	Log(LogLevel::Info, item...);
}

template<typename... Args>
inline void Logger::Warn(Args&&... item)
{
	Log(LogLevel::Warn, item...);
}

template<typename... Args>
inline void Logger::Error(Args&&... item)
{
	Log(LogLevel::Error, item...);
}

template<typename... Args>
inline void Logger::Critical(Args&&... item)
{
	Log(LogLevel::Critical, item...);
}
