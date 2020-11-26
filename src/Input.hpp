#pragma once
#include <algorithm>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

struct Point {
  int x, y;
  Point(int x = 0, int y = 0) : x(x), y(y) {}
};

struct Center {
  virtual Point center() const = 0;
};

struct Terminal : public Point, public Center {
  std::string name;
  Terminal(const std::string &name) : Point(), name(name) {}
  Point center() const override { return Point(x, y); }
};

struct Hardrectilinear : public Point, public Center {
  std::string name;
  int width, height;
  bool rotated;
  Hardrectilinear(const std::string &name)
      : Point(), name(name), width(0), height(0), rotated(false) {}

  int getWidth(bool rotate) const { return rotate ? height : width; }
  int getHeight(bool rotate) const { return rotate ? width : height; }
  int getWidth() const { return getWidth(rotated); }
  int getHeight() const { return getHeight(rotated); }
  Point center() const override {
    int xc = getWidth() / 2;
    int yc = getHeight() / 2;
    return Point(x + xc, y + yc);
  }

  bool isIntersect(const Hardrectilinear &b) const {
    int xLD = std::max(x, b.x);
    int yLD = std::max(y, b.y);
    int xRU = std::min(x + getWidth(), b.x + b.getWidth());
    int yRU = std::min(y + getHeight(), b.y + b.getHeight());
    return xLD < xRU && yLD < yRU;
  }
};

#include "Nodes.hpp"

struct Net {
  std::vector<Terminal *> terminals;
  std::vector<Hardrectilinear *> hardrectilinears;
  int HPWL() const {
    int xmin = std::numeric_limits<int>::max(),
        ymin = std::numeric_limits<int>::max();
    int xmax = 0, ymax = 0;
    for (auto t : terminals) {
      auto p = t->center();
      xmin = std::min(xmin, p.x);
      ymin = std::min(ymin, p.y);
      xmax = std::max(xmax, p.x);
      ymax = std::max(ymax, p.y);
    }
    for (auto h : hardrectilinears) {
      auto p = h->center();
      xmin = std::min(xmin, p.x);
      ymin = std::min(ymin, p.y);
      xmax = std::max(xmax, p.x);
      ymax = std::max(ymax, p.y);
    }
    return xmax - xmin + ymax - ymin;
  }
  int HPWL_in_tree(const std::unordered_map<const Hardrectilinear *, BaseNode *>
                       &baseNode) const {
    int xmin = std::numeric_limits<int>::max(),
        ymin = std::numeric_limits<int>::max();
    int xmax = 0, ymax = 0;
    for (auto t : terminals) {
      auto p = t->center();
      xmin = std::min(xmin, p.x);
      ymin = std::min(ymin, p.y);
      xmax = std::max(xmax, p.x);
      ymax = std::max(ymax, p.y);
    }
    for (auto h : hardrectilinears) {
      auto p = baseNode.at(h)->center();
      xmin = std::min(xmin, p.x);
      ymin = std::min(ymin, p.y);
      xmax = std::max(xmax, p.x);
      ymax = std::max(ymax, p.y);
    }
    return xmax - xmin + ymax - ymin;
  }
};