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
#else
#error "could not identify compiler"
#endif

Lemur::ConsoleLogger::ConsoleLogger()
{
}

void Lemur::ConsoleLogger::Debug(const char* origin, const char* message)
{
	Log(origin, message, "DEBUG", CL_CYAN);
}

void Lemur::ConsoleLogger::Info(const char* origin, const char* message)
{
	Log(origin, message, "INFO", CL_WHITE);
}

void Lemur::ConsoleLogger::Warning(const char* origin, const char* message)
{
	Log(origin, message, "WARNING" ,CL_YELLOW);
}

void Lemur::ConsoleLogger::Error(const char* origin, const char* message)
{
	Log(origin, message, "ERROR", CL_RED);

}

Lemur::ConsoleLogger::~ConsoleLogger()
{
}

#ifdef _WIN32
#include <windows.h>
void Lemur::ConsoleLogger::Log(const char* origin, const char* message, const char* type, Color color)
{
	using namespace std;
	// Set the color
	WORD ccolor = FOREGROUND_INTENSITY;
	if (color == CL_RED) ccolor |= FOREGROUND_RED;
	else if (color == CL_GREEN) ccolor |= FOREGROUND_GREEN;
	else if (color == CL_BLUE) ccolor |= FOREGROUND_BLUE;
	else if (color == CL_CYAN) ccolor |= (FOREGROUND_BLUE | FOREGROUND_GREEN);
	else if (color == CL_YELLOW) ccolor |= (FOREGROUND_GREEN | FOREGROUND_RED);
	//else white
	else ccolor |= (FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
	
	// Get handle to standard output
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  
	//save the current console info
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);

	//set console output to the supplied color
	SetConsoleTextAttribute(hConsole, ccolor);
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
void Lemur::ConsoleLogger::Log(const char* origin, const char* message, const char* type, Color color)
{
	using namespace std;
	cout << std::left
		<< setw(15) << string("\033[1;")+ color +"m" + type
		<< setw(22) << return_current_time_and_date()
		<< setw(20) << origin
		<< setw(30) << message
		<< "\033[0m\n" << std::right;
}
#endif
