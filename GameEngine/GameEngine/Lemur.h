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


namespace Lemur {
inline std::string to_string(const char* str)
{
	return str;
}


template <class T>
std::string to_string_resolver(T t)
{
	using std::to_string;
	using Lemur::to_string;
	return to_string(t);
}

namespace impl
{

template <class Fn>
void execute_lambda(Fn&& fn)
{
#ifndef NDEBUG
	std::forward<Fn>(fn)();
#endif
}

}// namepace impl
}// namespace Lemur


#define DEB_ONLY_BEGIN Lemur::impl::execute_lambda([&]{
#define DEB_ONLY_END });


#define CSTR(a, b) \
(std::string(a) + Lemur::to_string_resolver(b)).c_str()

#define CSTR2(a, b, c) \
(std::string(a) + Lemur::to_string_resolver(b) + std::string(c)).c_str()
