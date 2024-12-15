#ifndef USAGE_HPP
#define USAGE_HPP

#include <iostream>
#include <string>

void showUsage(const std::string &programName)
{
  std::cout << "Usage: " << programName << " [options]\n";
  std::cout << "Options:\n";
  std::cout << "  -f            Floating mode (can also be specified in component init).\n";
  std::cout << "  -F            Fullscreen mode.\n";
  std::cout << "                If either are not specified, defaults to managed by Window Manager.\n";
  std::cout << "                (Mutually exclusive: priority order is wm > -f > -F).\n\n";
  std::cout << "  -c <path>     Specify a component path.\n";
  std::cout << "  -C <path>     Specify a configuration file path (takes precedence over -c).\n\n";
  std::cout << "  -g            Grab the keyboard (Will grab by default if -F).\n\n";
  std::cout << "  -w <width>    Specify window width (used in floating mode).\n";
  std::cout << "  -h <height>   Specify window height (used in floating mode).\n\n";
  std::cout << "  --help        Show this help message.\n";
}
void exitWithUsage()
{
  showUsage("drackd");
  exit(EXIT_FAILURE);
}

#endif