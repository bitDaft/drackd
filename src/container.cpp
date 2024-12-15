#include "container.hpp"

Container::Container(int x, int y)
    : x(x), y(y)
{
}

Container::~Container()
{
}

void Container::setDimension(float width, float height, bool abs)
{
  width = width
}

void Container::updateSizes(int windowWidth, int windowHeight)
{
  absWidth = windowWidth * width;
  absHeight = windowHeight * height;
}
