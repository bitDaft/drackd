#include <iostream>

#include <getopt.h>
#include <unistd.h>
#include <sys/select.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include "application.hpp"
#include "utils.hpp"

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

void Application::init()
{
  display = XOpenDisplay(NULL);
  if (!display)
    printAndDie(false, "Unable to open X display");
  screen = DefaultScreen(display);
  root = RootWindow(display, screen);

  XSetWindowAttributes swa;
  swa.override_redirect = False;
  swa.background_pixel = BlackPixel(display, screen);
  swa.event_mask = SubstructureRedirectMask | SubstructureNotifyMask | StructureNotifyMask | ExposureMask |
                   PropertyChangeMask | KeyPressMask | VisibilityChangeMask | PropertyChangeMask | ResizeRedirectMask;
  window = XCreateWindow(display, root, 0, 0, appConfig.width, appConfig.height, 0,
                         CopyFromParent, CopyFromParent, CopyFromParent,
                         CWOverrideRedirect | CWBackPixel | CWEventMask, &swa);

  gc = XCreateGC(display, window, 0, NULL);
  // pixmap = XCreatePixmap(display, root, appConfig.width, appConfig.height, DefaultDepth(display, screen));

  clip = XInternAtom(display, "CLIPBOARD", False);
  utf8 = XInternAtom(display, "UTF8_STRING", False);
}

void Application::run()
{
  init();
  XSetForeground(display, gc, WhitePixel(display, screen));
  setupWindow();
  if (appConfig.grabKeyboard)
    grabKeyboard(display);
  XEvent event;
  bool aa = true;
  while (aa)
  {
    XNextEvent(display, &event);
    aa = handleEvent(event);
    render();
    XFlush(display);
  }
}
void Application::setupWindow()
{
  XClassHint ch = {"drackd", "drackd"};
  XStoreName(display, window, "drackd");
  XSetClassHint(display, window, &ch);

  struct
  {
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long inputMode;
    unsigned long status;
  } hints = {2, 0, 0, 0, 0}; // Disable decorations

  XChangeProperty(display, window, XInternAtom(display, "_MOTIF_WM_HINTS", False), XA_ATOM, 32, PropModeReplace,
                  (unsigned char *)&hints, 5);

  Atom dump;
  XChangeProperty(display, window, XInternAtom(display, "_NET_WM_WINDOW_TYPE", False), XA_ATOM, 32,
                  PropModeReplace, (unsigned char *)&(dump = XInternAtom(display, "_NET_WM_WINDOW_TYPE_NORMAL", False)), 1);
  if (appConfig.fullscreen)
  {
    XChangeProperty(display, window, XInternAtom(display, "_NET_WM_STATE", False), XA_ATOM, 32,
                    PropModeReplace, (unsigned char *)&(dump = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False)), 1);
  }
  XSizeHints *sh;
  if (appConfig.floating)
  {
    // make the window non-resizable to make it floating
    sh = XAllocSizeHints();
    sh->flags = PMinSize | PMaxSize;
    sh->min_width = sh->max_width = appConfig.width;
    sh->min_height = sh->max_height = appConfig.height;
    XSetWMNormalHints(display, window, sh);
    XFree(sh);
  }

  XMapWindow(display, window);
  XEvent e;
  while (true)
  {
    XNextEvent(display, &e);
    if (e.type == MapNotify)
      break;
  }

  if (appConfig.floating)
  {
    // Make the window resizable again after floating
    sh = XAllocSizeHints();
    sh->flags = PMinSize | PMaxSize | PPosition;
    sh->min_width = 10;
    sh->max_width = INT32_MAX;
    sh->min_height = 10;
    sh->max_height = INT32_MAX;
    XSetWMNormalHints(display, window, sh);
    XFree(sh);
  }
}

bool Application::handleEvent(XEvent &event)
{
  if (event.type == KeyPress)
  {
    std::cout << "key pressed\n";
    KeySym key = XLookupKeysym(&event.xkey, 0); // Get the key symbol
    if (key == XK_Escape)
    {               // Check if the Escape key was pressed
      return false; // Exit the loop
    }
  }
  else
  {
    std::cout << "Unhandled event type: " << event.type << std::endl;
    return true;
  }
}

void Application::render()
{
  XDrawLine(display, window, gc, 10, 60, 180, 20);
}