#include <iostream>

#include <getopt.h>
#include <unistd.h>
#include <sys/select.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include "application.hpp"
#include "utils.hpp"

// forward declaring needed function
bool parseArguments(AppConfig &appConfig, int argc, char **argv, std::string &errorMessage);
void checkAndReadStdin(std::string &input);

Application::Application(int argc, char *argv[])
{
  if (!setlocale(LC_CTYPE, "") || !XSupportsLocale())
    std::cerr << "warning: no locale support\n";

  std::string errorMsg;
  if (argc == 1 || !parseArguments(appConfig, argc, argv, errorMsg))
    printAndDie(true, errorMsg);

  checkAndReadStdin(stdinData);
}

Application::~Application()
{
  XDestroyWindow(display, window);
  XCloseDisplay(display);
}

void Application::run()
{
  createAppWindow();
  if (appConfig.grabKeyboard)
    grabKeyboard();
  handleWindowEvents();
}

void Application::createAppWindow()
{
  display = XOpenDisplay(NULL);
  if (!display)
    printAndDie(false, "Unable to open X display");

  screen = DefaultScreen(display);
  root = RootWindow(display, screen);

  clip = XInternAtom(display, "CLIPBOARD", False);
  utf8 = XInternAtom(display, "UTF8_STRING", False);

  unsigned long black = BlackPixel(display, screen);
  unsigned long white = WhitePixel(display, screen);

  XSetWindowAttributes swa;
  swa.override_redirect = False;
  swa.background_pixel = black;
  swa.event_mask = SubstructureRedirectMask | SubstructureNotifyMask | StructureNotifyMask | ExposureMask | KeyPressMask | VisibilityChangeMask | PropertyChangeMask;
  window = XCreateWindow(display, root, 0, 0, appConfig.width, appConfig.height, 0,
                         CopyFromParent, CopyFromParent, CopyFromParent,
                         CWOverrideRedirect | CWBackPixel | CWEventMask, &swa);

  Atom wmState = XInternAtom(display, "_NET_WM_STATE", False);
  Atom fullscreenAtom = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);
  // Atom wmAllowedActions = XInternAtom(display, "_NET_WM_ALLOWED_ACTIONS", False);
  // Atom wmActionFullscreen = XInternAtom(display, "_NET_WM_ACTION_FULLSCREEN", False);
  // Atom atoms[2] = {XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False), None};
  // Atom wmActionFullscreen = XInternAtom(display, "_NET_WM_ACTION_FULLSCREEN", False);

  // Disallow fullscreen toggling
  XChangeProperty(display, window, wmState, XA_ATOM, 32,
                  PropModeReplace, (unsigned char *)&fullscreenAtom, 1);
  // XChangeProperty(display, window, wmAllowedActions, XA_ATOM, 32,
  //                 PropModeReplace, (unsigned char *)&wmActionFullscreen, 0);

  // window = XCreateSimpleWindow(display, root, 0, 0, appConfig.width, appConfig.height, 0, black, black);
  XClassHint ch = {"drackd", "drackd"};
  XStoreName(display, window, "drackd");
  XSetClassHint(display, window, &ch);

  XWindowAttributes wa;
  if (!XGetWindowAttributes(display, window, &wa))
    printAndDie("could not get created window attributes");

  gc = XCreateGC(display, window, 0, NULL);
  // pixmap = XCreatePixmap(display, root, wa.width, wa.height, DefaultDepth(display, screen));

  // XSelectInput(display, window, StructureNotifyMask | ExposureMask | KeyPressMask | VisibilityChangeMask);
  XMapWindow(display, window);

  XSetForeground(display, gc, white);

  XEvent e;
  for (;;)
  {
    XNextEvent(display, &e);
    if (e.type == MapNotify)
      break;
  }
}

void Application::handleWindowEvents()
{
  XEvent event;
  while (true)
  {
    XNextEvent(display, &event);
    if (event.type == KeyPress)
    {
      std::cout << "key pressed\n";
      KeySym key = XLookupKeysym(&event.xkey, 0); // Get the key symbol
      if (key == XK_Escape)
      {        // Check if the Escape key was pressed
        break; // Exit the loop
      }
    }
    else if (event.type == ConfigureNotify && appConfig.fullscreen)
    {
      // Force fullscreen geometry
      std::cout << "geochange\n";
      // makeFullscreen();
      // XMoveResizeWindow(display, window, 0, 0,
      //                   DisplayWidth(display, screen),
      //                   DisplayHeight(display, screen));
    }
    else if (event.type == PropertyNotify && appConfig.fullscreen)
    {
      // Force fullscreen geometry
      std::cout << "PropertyNotify\n";
      Atom wmState = XInternAtom(display, "_NET_WM_STATE", True);
      Atom fullscreenAtom = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", True);
      Atom type;
      int format;
      unsigned long length;
      unsigned long after;
      unsigned char *dp;
      int status = XGetWindowProperty(display, window, wmState,
                                      0L, (~0L), 0,
                                      AnyPropertyType, &type, &format,
                                      &length, &after, &dp);
      if (status != Success)
        continue;
      int found = 0;
      for (int i = 0; i < length; i++)
      {
        std::string atomName = XGetAtomName(display, ((Atom *)dp)[i]);
        if (std::string("_NET_WM_STATE_FULLSCREEN") == atomName)
        {
          found = true;
          break;
        }
      }
      if (!found)
      {
        makeFullscreen();
      }
      XFree(dp);
    }
    else
    {
      // std::cout << "Unhandled event type: " << event.type << std::endl;
    }
    render();
    XFlush(display);
  }
}

void Application::grabKeyboard()
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

void Application::makeFullscreen()
{
  if (!appConfig.fullscreen)
    return;
  std::cout << "Fullscreen\n";
  XEvent xev;
  Atom wmState = XInternAtom(display, "_NET_WM_STATE", False);
  Atom fullscreenAtom = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);
  if (!fullscreenAtom)
  {
    std::cerr << "Failed to get _NET_WM_STATE_FULLSCREEN atom." << std::endl;
    exit(EXIT_FAILURE);
  }

  xev.type = ClientMessage;
  xev.xclient.window = window;
  xev.xclient.message_type = wmState;
  xev.xclient.format = 32;
  xev.xclient.data.l[0] = 1;
  xev.xclient.data.l[1] = fullscreenAtom;
  xev.xclient.data.l[2] = 0;

  int result = XSendEvent(display, root, False,
                          SubstructureRedirectMask | SubstructureNotifyMask,
                          &xev);
  if (!result)
  {
    std::cerr << "Failed to send fullscreen event." << std::endl;
  }
  else
  {
    std::cout << "Fullscreen event sent successfully." << std::endl;
  }
}

void Application::render()
{
  XDrawLine(display, window, gc, 10, 60, 180, 20);
}

void Application::updateSizes(int windowWidth, int windowHeight)
{
  appConfig.width = windowWidth;
  appConfig.height = windowHeight;

  // TODO: rerender everything
}

bool parseArguments(AppConfig &appConfig, int argc, char **argv, std::string &errorMessage)
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
