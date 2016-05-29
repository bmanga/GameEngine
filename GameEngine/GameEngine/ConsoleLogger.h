#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include <typeinfo>
#include "types.h"
#include <sstream>



namespace Lemur
{

std::string demangle(const char *abiName);
std::string filter_filename_location(std::string file);

template <class T>
std::string get_origin()
{
	return demangle(typeid(T).name());
}

template <class... Args>
std::string str(Args... args)
{
	std::stringstream ss;
	ss.precision(4);
	ss << std::fixed;
	int unused[] = { (ss << args, 0)... };
	return ss.str();

}

template <class... Args>
const char* cstr(Args... args)
{
	return str(args...).c_str();
}

class ConsoleLogger
{
public:
	enum class Color : Lemur::u16
	{
		RED    = 0x000C,
		GREEN  = 0x000A,
		BLUE   = 0x0009,
		WHITE  = 0x000F,
		CYAN   = 0x000B,
		YELLOW = 0x000E
	};

	static void Debug(const char* origin, const char* message);
	static void Info(const char* origin, const char* message);
	static void Warning(const char* origin, const char* message);
	static void Error(const char* origin, const char* message);

	static void Log(const char* origin, const char* message, const char* type, Color color = Color::WHITE);

};
}

