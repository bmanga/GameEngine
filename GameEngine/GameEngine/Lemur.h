#pragma once

#include <type_traits>
#include <string>

#include "ConsoleLogger.h"
#ifdef ENABLE_ASSERT_CL

#define CLASS_NAME (Lemur::get_origin<decltype(*this)>().c_str())

#define CODE_LOCATION \
(Lemur::filter_filename_location(__FILE__) + " L" + std::to_string(__LINE__)).c_str()

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

#define ASSERT_CLWARNING(condition, origin, message) \
if(!(condition)) \
{\
	Lemur::ConsoleLogger::Warning(origin, message);\
}\
else {}\

#define ASSERT_CLINFO(condition, origin, message) \
if(!(condition)) \
{\
	Lemur::ConsoleLogger::Info(origin, message);\
}\
else {}\

#else
#define ASSERT_CLDEBUG
#define ASSERT_CLERROR
#define ASSERT_CLWARNING
#define ASSERT_CLINFO
#endif


inline std::string to_string(const char* str)
{
	return str;
}

#define CSTR(a, b) \
(std::string(a) + to_string(b)).c_str()

#define CSTR2(a, b, c) \
(std::string(a) + to_string(b) + std::string(c)).c_str()
