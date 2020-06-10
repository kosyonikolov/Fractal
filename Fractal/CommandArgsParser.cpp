#include "CommandArgsParser.h"
#include "CommandArgsParser.h"

#include <map>
#include <iostream>
#include <sstream>

void CommandArgsParser::Parse(char** args, const uint32_t count)
{
	std::map<std::string, std::string> values;

	for (uint32_t i = 0; i < count; i++)
	{
		const std::string currentArg = args[i];
		if (currentArg.length() < 2)
		{
			continue; // nothing to do
		}

		if (currentArg[0] == '-')
		{
			// flag
			if (i == count - 1)
			{
				// last arg -> flag with no value
				break;
			}

			// value
			const std::string nextArg = args[i + 1];
			//values.insert(currentArg.substr(1), nextArg);
			i++;
		}
		else
		{
			std::cerr << "Value with no flag: " << currentArg << "\n";
		}
	}

	// parse image size
	{
		auto it = values.find("s");
		if (it == values.end())
		{
			it = values.find("size");
		}
		
		if (it != values.end())
		{
			std::stringstream ss(it->second);

			uint32_t w, h;
			char c;
			if (ss >> w >> c >> h)
			{
				this->width = w;
				this->height = h;
			}
		}
	}

	// parse complex plane size
	{
		auto it = values.find("r");
		if (it != values.end())
		{
			std::stringstream ss(it->second);
			double xs, xe, ys, ye;
			char c;
			if (ss >> xs >> c >> xe >> ys >> c >> ye)
			{
				this->compOffX = xs;
				this->compOffY = ys;
				this->compWidth = xe - xs;
				this->compHeight = ye - ys;
			}
		}
	}

	// parse thread count
	{
		auto it = values.find("t");
		if (it != values.end())
		{
			it = values.find("threads");
		}

		if (it != values.end())
		{
			try
			{
				int count = std::stoi(it->second);
				this->threadCount = count;
			}
			catch (std::exception& e)
			{
				std::cerr << "Bad thread count format\n";
			}
		}
	}
}
