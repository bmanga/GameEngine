#include "ConsoleLogger.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <stdio.h>
#include <sstream>
#include <ctime>
std::string return_current_time_and_date()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;

	ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
	return ss.str();
}
#if defined(__clang__) || defined (__GNUC__)
#include <cxxabi.h>
std::string demangle(const char *abiName)
{
	int status;
	char *ret = abi::__cxa_demangle(abiName, 0, 0, &status);

	std::string retval(ret);
	/* NOTE: must free() the returned char when done with it! */
	delete[] ret;
	return retval;
}


#elif defined(_MSC_VER)
#include <sstream>
#include <algorithm>
#include <iterator>
#include <string>
//awful piece of code. Not very scalable either but it gets the job done 
//for the time being
//TODO: improve this, maybe extend compiler support

std::string Lemur::demangle(const char* abiName)
{
	std::stringstream stringStream(abiName);
	std::string line;
	std::string demangled;
	while (std::getline(stringStream, line, ','))
	{
		if (line == "class") continue;
		std::size_t prev = 0, pos;
		while ((pos = line.find_first_of(" ", prev)) != ::std::string::npos)
		{
			if (pos > prev)
			{
				::std::string tmp = line.substr(prev, pos - prev);
				if (tmp != "class")	demangled.append(','+ tmp);
				
			}
			prev = pos + 1;

		}
		if (prev < line.length())
			demangled.append(line.substr(prev, std::string::npos));
	}
	return demangled;
}

std::string Lemur::filter_filename_location(std::string file)
{
	auto pos = file.find_last_of("\\/");
	return file.substr(pos + 1, std::string::npos);
}
#else
#error "could not identify compiler"
#endif

void Lemur::ConsoleLogger::Debug(const char* origin, const char* message)
{
	Log(origin, message, "DEBUG", Color::CYAN);
}

void Lemur::ConsoleLogger::Info(const char* origin, const char* message)
{
	Log(origin, message, "INFO", Color::WHITE);
}

void Lemur::ConsoleLogger::Warning(const char* origin, const char* message)
{
	Log(origin, message, "WARNING" ,Color::YELLOW);
}

void Lemur::ConsoleLogger::Error(const char* origin, const char* message)
{
	Log(origin, message, "ERROR", Color::RED);

}

#ifdef _WIN32
#include <windows.h>
void Lemur::ConsoleLogger::Log(const char* origin, const char* message, 
	const char* type, Color color)
{
	using namespace std;
	
	// Get handle to standard output
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  
	//save the current console info
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);

	//set console output to the supplied color
	SetConsoleTextAttribute(hConsole, static_cast<WORD>(color));
	cout << std::left
		<< setw(8) << type
		<< setw(22) << return_current_time_and_date()
		<< setw(20) << origin
		<< setw(30) << message
		<< std::endl << std::right;

	//restore the console settings
	SetConsoleTextAttribute(hConsole, csbi.wAttributes);
}
#else
void Lemur::ConsoleLogger::Log(const char* origin, const char* message, const char* type, Color col)
{
	using namespace std;

	const char* color;
	switch (col)
	{
	case Color::RED : 
		color = "91"; break;
	case Color::GREEN :
		color = "92"; break;
	case Color::YELLOW :
		color = "93"; break;
	case Color::BLUE :
		color = "94"; break;
	case Color::CYAN :
		color = "96"; break;
	default:
		color = "97";

	}
	cout << std::left
		<< setw(15) << string("\033[1;")+ color +"m" + type
		<< setw(22) << return_current_time_and_date()
		<< setw(20) << origin
		<< setw(30) << message
		<< "\033[0m\n" << std::right;
}
#endif
