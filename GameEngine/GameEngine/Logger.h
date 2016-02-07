#pragma once
#include <iostream>
#include <iomanip>
#include <string>

namespace Lemur
{
	std::string demangle(const char *abiName);


	class ConsoleLogger
	{
	public:
		ConsoleLogger();

		void logDebug(const char* message);
		void logInfo(const char* message);
		void logWarning(const char* message);
		void logError(const char* message);
		~ConsoleLogger();
	};
}

