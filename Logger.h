/*
 * Logger.h
 *
 *  Created on: Jun 2, 2016
 *      Author: michal
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>

void log_write(std::string&& s);

void log_write(const char* s);

void log_writeln(std::string&& s);


#endif /* LOGGER_H_ */
