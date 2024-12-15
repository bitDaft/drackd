#ifndef utils_HPP
#define utils_HPP

#include <iostream>
#include <string>

#include "usage.hpp"

void printAndDie(bool printUsage = false, const std::string &message = "")
{
  if (message.length() > 0)
    std::cout << message << std::endl;
  if (printUsage)
    showUsage("drackd");
  exit(EXIT_FAILURE);
}

#endif