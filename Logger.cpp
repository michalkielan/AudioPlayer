/*
 * Logger.cpp
 *
 *  Created on: Jun 2, 2016
 *      Author: michal
 */

#include "Logger.h"

void log_write(std::string&& s)
{
  std::cout << s;
}

void log_write(const char* s)
{
  std::cout << s;
}

void log_writeln(std::string&& s)
{
  std::cout << s << std::endl;
}
