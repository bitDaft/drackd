#include <iostream>

#include "application.hpp"

int main(int argc, char *argv[])
{

  Application app(argc, argv);
  app.run();

  // Further initialization logic here...
  return EXIT_SUCCESS;
}