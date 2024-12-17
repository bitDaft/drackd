#ifndef utils_HPP
#define utils_HPP

#include <iostream>
#include <string>

#include <X11/Xlib.h>

#include "usage.hpp"

void printAndDie(bool printUsage = false, const std::string &message = "")
{
  if (message.length() > 0)
    std::cout << message << std::endl;
  if (printUsage)
    showUsage("drackd");
  exit(EXIT_FAILURE);
}

bool parseArguments(AppConfig &appConfig, int argc, char **argv, std::string &errorMessage)
{
  int opt;
  while ((opt = getopt(argc, argv, "C:fFc:w:h:g")) != -1)
  {
    switch (opt)
    {
    case 'f':
      appConfig.floating = true;
      appConfig.managed = false;
      appConfig.fullscreen = false;
      break;
    case 'F':
      appConfig.fullscreen = !appConfig.floating;
      appConfig.managed = false;
      break;
    case 'c':
      if (appConfig.usingConfigFile)
        break;
      appConfig.usingConfigFile = false;
      appConfig.filePath = optarg;
      break;
    case 'C':
      appConfig.usingConfigFile = true;
      appConfig.filePath = optarg;

      break;
    case 'g':
      appConfig.grabKeyboard = true;
      break;
    case 'w':
      appConfig.width = std::stoi(optarg);
      break;
    case 'h':
      appConfig.height = std::stoi(optarg);
      break;
    default:
      return false;
    }
  }

  // Resolve mutually exclusive arguments
  if (appConfig.filePath.empty())
  {
    errorMessage = "Error: You must specify either -c <path> or -C <path>\n\n";
    return false;
  }

  // Handle missing width/height when floating
  if (appConfig.floating && (appConfig.width == 0 || appConfig.height == 0))
  {
    errorMessage = "Error: Floating mode requires both -w and -h options.\n\n";
    return false;
  }

#ifdef _DEBUG_
  // Debug: Display parsed configuration
  std::cout << "Configuration:\n";
  std::cout << "  Mode: " << (appConfig.fullscreen ? "Fullscreen" : (appConfig.floating ? "Floating" : "Managed")) << "\n";
  std::cout << "  File Path: " << appConfig.filePath << "\n";
  if (appConfig.floating)
  {
    std::cout << "  Window Dimensions: " << appConfig.width << "x" << appConfig.height << "\n";
  }
  std::cout << "  Grab Keyboard: " << (appConfig.grabKeyboard ? "Yes" : "No") << "\n";
#endif
  return true;
}

void checkAndReadStdin(std::string &input)
{
  fd_set readfds;
  struct timeval timeout;

  FD_ZERO(&readfds);
  FD_SET(STDIN_FILENO, &readfds);

  timeout.tv_sec = 0;
  timeout.tv_usec = 0;

  int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);

  if (result > 0 && FD_ISSET(STDIN_FILENO, &readfds))
  {
    char buffer[1024];
    while (std::cin.read(buffer, sizeof(buffer)))
    {
      input.append(buffer, std::cin.gcount());
    }
    input.append(buffer, std::cin.gcount());
  }

#ifdef _DEBUG_
  if (input.length() > 0)
  {
    std::cout << "Stdin Data Received: " << input << "\n";
  }
#endif
}

void grabKeyboard(Display *display)
{
  struct timespec ts = {.tv_sec = 0, .tv_nsec = 1000000};
  int i;

  for (i = 0; i < 1000; i++)
  {
    if (XGrabKeyboard(display, DefaultRootWindow(display), True, GrabModeAsync,
                      GrabModeAsync, CurrentTime) == GrabSuccess)
      return;
    nanosleep(&ts, NULL);
  }
  printAndDie(false, "Could not grab keyboard");
}

void grabfocus(Display *display, Window &window)
{
  struct timespec ts = {.tv_sec = 0, .tv_nsec = 10000000};
  Window focuswin;
  int i, revertwin;

  for (i = 0; i < 100; ++i)
  {
    XGetInputFocus(display, &focuswin, &revertwin);
    if (focuswin == window)
      return;
    XSetInputFocus(display, window, RevertToParent, CurrentTime);
    nanosleep(&ts, NULL);
  }
  printAndDie(false, "Could not grab focus");
}

#endif