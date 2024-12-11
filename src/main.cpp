#include <iostream>
#include <string>
#include <string>

#include <getopt.h>

struct AppConfig
{
  bool floating = false;
  bool fullscreen = false;
  bool managed = true;
  bool grabKeyboard = false;
  bool readInput = false;
  bool usingConfigFile = false;
  std::string filePath;
  int width = 0;
  int height = 0;
};

AppConfig appConfig;

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
  std::cout << "  -i            Read input from stdin (will process only if available).\n";
  std::cout << "                Will not wait for stdin, data should be ready for stdin, if not will skip stdin.\n\n";
  std::cout << "  --help        Show this help message.\n";
}

void parseArguments(int argc, char *argv[])
{
  int opt;
  while ((opt = getopt(argc, argv, "fFC:c:w:h:gi")) != -1)
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
    case 'i':
      appConfig.readInput = true;
      break;
    default:
      showUsage(argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  // Resolve mutually exclusive arguments
  if (appConfig.filePath.empty())
  {
    std::cerr << "Error: You must specify either -c <path> or -C <path>\n\n";
    showUsage(argv[0]);
    exit(EXIT_FAILURE);
  }

  // Handle missing width/height when floating
  if (appConfig.floating && (appConfig.width == 0 || appConfig.height == 0))
  {
    std::cerr << "Error: Floating mode requires both -w and -h options.\n\n";
    showUsage(argv[0]);
    exit(EXIT_FAILURE);
  }

  // Debug: Display parsed configuration
  std::cout << "Configuration:\n";
  std::cout << "  Mode: " << (appConfig.fullscreen ? "Fullscreen" : (appConfig.floating ? "Floating" : "Managed")) << "\n";
  std::cout << "  File Path: " << appConfig.filePath << "\n";
  if (appConfig.floating)
  {
    std::cout << "  Window Dimensions: " << appConfig.width << "x" << appConfig.height << "\n";
  }
  std::cout << "  Grab Keyboard: " << (appConfig.grabKeyboard ? "Yes" : "No") << "\n";
  std::cout << "  Read Input: " << (appConfig.readInput ? "Yes" : "No") << "\n";
}

bool checkAndReadStdin(std::string &input)
{
  if (std::cin.peek() != EOF)
  {
    char buffer[1024];
    while (std::cin.read(buffer, sizeof(buffer)))
    {
      input.append(buffer, std::cin.gcount());
    }
    input.append(buffer, std::cin.gcount());
  }
  return input.length() > 0;
}

int main(int argc, char *argv[])
{
  if (argc == 1)
  {
    showUsage(argv[0]);
    return EXIT_FAILURE;
  }

  parseArguments(argc, argv);

  std::string stdinData;

  if (appConfig.readInput)
  {
    if (checkAndReadStdin(stdinData))
    {
      std::cout << "Stdin Data Received:\n"
                << stdinData << "\n";
    }
    else
    {
      std::cout << "No data on stdin; skipping input processing.\n";
    }
  }

  // Further initialization logic here...
  return EXIT_SUCCESS;
}