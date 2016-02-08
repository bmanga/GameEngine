#pragma once
#include <iostream>
#include <iomanip>
#include <string>

#define ENABLE_ASSERT_CL


namespace Lemur
{
#ifdef ENABLE_ASSERT_CL
#include <typeinfo>

std::string demangle(const char *abiName);

template <class T>
std::string get_origin()
{
	return demangle(typeid(T).name());
}

#define CLASS_NAME (Lemur::get_origin<decltype(*this)>().c_str())
#define ASSERT_CLDEBUG(condition, origin, message) \
if(!(condition)) \
{\
	Lemur::ConsoleLogger::Debug(origin, message);\
}\
else {}\

#define ASSERT_CLERROR(condition, origin, message) \
if(!(condition)) \
{\
	Lemur::ConsoleLogger::Error(origin, message);\
}\
else {}\

#else
#define ASSERT_CLDEBUG
#define ASSERT_CLERROR
#endif


#define CL_RED    "91"
#define CL_GREEN  "92"
#define CL_YELLOW "93"
#define CL_BLUE   "94"
#define CL_PINK   "95"
#define CL_CYAN   "96"
#define CL_WHITE  "97"

class ConsoleLogger
{
public:
	using Color = const char*;
	ConsoleLogger();

	static void Debug(const char* origin, const char* message);
	static void Info(const char* origin, const char* message);
	static void Warning(const char* origin, const char* message);
	static void Error(const char* origin, const char* message);

	static void Log(const char* origin, const char* message, const char* type, Color color = CL_WHITE);
	~ConsoleLogger();
};
}

