#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <string>
#include <vector>

#include <X11/Xlib.h>

#include "appConfig.hpp"

class Application
{
public:
  Application(int argc, char *argv[]);
  ~Application();

  void run();

private:
  std::string stdinData;

  AppConfig appConfig;

  Display *display;
  Window root, window;
  int screen;
  Atom clip, utf8;
  GC gc;
  Drawable pixmap;

private:
  void init();
  void setupWindow();
  bool handleEvent(XEvent &event);
  void render();
};

#endif