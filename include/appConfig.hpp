#ifndef APPCONFIG_HPP
#define APPCONFIG_HPP

#include <string>

struct AppConfig
{
  bool floating = false;
  bool fullscreen = false;
  bool managed = true;
  bool grabKeyboard = false;
  bool usingConfigFile = false;
  int width = 800;
  int height = 600;
  std::string filePath;
};

#endif