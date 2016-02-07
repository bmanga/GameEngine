#include "Logger.h"

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
		while ((pos = line.find_first_of(" ", prev)) != std::string::npos)
		{
			if (pos > prev)
			{
				std::string tmp = line.substr(prev, pos - prev);
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


Lemur::ConsoleLogger::~ConsoleLogger()
{
}
