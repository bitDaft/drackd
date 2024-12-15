#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <string>
#include <vector>

#include "appConfig.hpp"

class Application
{
public:
  Application(int argc, char *argv[]);
  ~Application();

private:
  std::string stdinData;
  AppConfig appConfig;

private:
  void createAppWindow();
  void updateSizes(int windowWidth, int windowHeight);
};

#endif