#include <iostream>

#include <getopt.h>
#include <unistd.h>
#include <sys/select.h>

#include "application.hpp"
#include "usage.hpp"

// forward declaring needed function
bool parseArguments(AppConfig &appConfig, int argc, char **argv);
void checkAndReadStdin(std::string &input);

Application::Application(int argc, char *argv[])
{
  if (argc == 1 || !parseArguments(appConfig, argc, argv))
    exitWithUsage();

  checkAndReadStdin(stdinData);
}

Application::~Application()
{
}

void Application::createAppWindow()
{
}

void Application::updateSizes(int windowWidth, int windowHeight)
{
  appConfig.width = windowWidth;
  appConfig.height = windowHeight;

  // TODO: rerender everything
}

bool parseArguments(AppConfig &appConfig, int argc, char **argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "fFC:c:w:h:g")) != -1)
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
    std::cerr << "Error: You must specify either -c <path> or -C <path>\n\n";
    return false;
  }

  // Handle missing width/height when floating
  if (appConfig.floating && (appConfig.width == 0 || appConfig.height == 0))
  {
    std::cerr << "Error: Floating mode requires both -w and -h options.\n\n";
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
