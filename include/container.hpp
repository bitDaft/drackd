#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include <string>
#include <vector>

#include "component.hpp"

class Container
{
public:
  Container(int x, int y);
  ~Container();

  void setDimension(float width, float height, bool abs);

  void updateSizes(int windowWidth, int windowHeight);

private:
  int x;
  int y;
  float width;
  float height;
  int absWidth;
  int absHeight;
  std::vector<Component> components;
};

#endif