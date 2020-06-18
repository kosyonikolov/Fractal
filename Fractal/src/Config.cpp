#include "Config.h"

#include <map>
#include <iostream>
#include <sstream>

bool parseConfig(char** args, const uint32_t count, Config * outConfig)
{
	std::map<std::string, std::string> values;

	for (uint32_t i = 0; i < count; i++)
	{
		const std::string currentArg = args[i];
		if (currentArg.length() < 2)
		{
			std::cerr << __FUNCTION__ << ": Skipped option " << currentArg << "\n";
			continue; // nothing to do
		}

		if (currentArg[0] == '-')
		{
			const std::string option = currentArg.substr(1);

			if (option == "q" || option == "quiet")
			{
				outConfig->verbosity = Verbosity::Silent;
				continue;
			}

			// flag
			if (i == count - 1)
			{
				// last arg -> flag with no value
				break;
			}

			// value
			const std::string nextArg = args[i + 1];
			values[option] = nextArg;
			i++;
		}
		else
		{
			std::cerr << "Value with no flag: " << currentArg << "\n";
		}
	}

	// find the value for the given pair of short/long options
	auto findValue = [&](const std::string & shortOpt, const std::string & longOpt)
	{
		auto it = values.find(shortOpt);
		if (it == values.end())
		{
			it = values.find(longOpt);
		}

		return (it == values.end()) ? "" : it->second;		
	};

	// parse image size
	{
		const std::string imageSizeStr = findValue("s", "size");
		if (imageSizeStr != "")
		{
			std::stringstream ss(imageSizeStr);

			uint32_t w, h;
			char c;
			if (ss >> w >> c >> h)
			{
				outConfig->width = w;
				outConfig->height = h;
			}
			else
			{
				std::cerr << __FUNCTION__ << ": Couldn't parse image size from " << imageSizeStr << "\n";
				return false;
			}
		}	
	}

	// parse complex plane size
	{
		const std::string complexSizeStr = findValue("r", "region");
		if (complexSizeStr != "")
		{
			std::stringstream ss(complexSizeStr);
			double xs, xe, ys, ye;
			char c;
			if (ss >> xs >> c >> xe >> c >> ys >> c >> ye)
			{
				outConfig->compOffX = xs;
				outConfig->compOffY = ys;
				outConfig->compWidth = xe - xs;
				outConfig->compHeight = ye - ys;
			}
			else
			{
				std::cerr << __FUNCTION__ << ": Couldn't parse complex size from " << complexSizeStr << "\n";
				return false;
			}
		}
	}

	// parse thread count
	{
		const std::string threadCountStr = findValue("t", "threads");
		if (threadCountStr != "")
		{
			try
			{
				int count = std::stoi(threadCountStr);
				outConfig->threadCount = count;
			}
			catch (std::exception& e)
			{
				std::cerr << __FUNCTION__ << ": Couldn't parse thread count from " << threadCountStr << "\n";
				return false;
			}
		}
	}

	// parse granularity
	{
		const std::string granularityStr = findValue("g", "granularity");
		if (granularityStr != "")
		{
			try
			{
				int g = std::stoi(granularityStr);
				outConfig->granularity = g;
			}
			catch (std::exception& e)
			{
				std::cerr << __FUNCTION__ << ": Couldn't parse granularity from " << granularityStr << "\n";
				return false;
			}
		}
	}

	// parse output file name
	{
		const std::string fileStr = findValue("o", "output");
		if (fileStr != "")
		{
			outConfig->outputFileName = fileStr;
		}
	}

	// parse verbosity
	{
		const std::string verbosityStr = findValue("v", "verbosity");
		if (verbosityStr != "")
		{
			try
			{
				int v = std::stoi(verbosityStr);
				outConfig->verbosity = Verbosity(v);
			}
			catch (std::exception& e)
			{
				std::cerr << __FUNCTION__ << ": Couldn't parse granularity from " << verbosityStr << "\n";
				return false;
			}
		}
	}

	return true;
}