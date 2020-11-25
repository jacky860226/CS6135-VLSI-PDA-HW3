#pragma once
#include "Input.hpp"

struct BaseNode : public Center {
  Hardrectilinear *rect;
  int x, y;
  bool rotated;
  BaseNode() {}
  BaseNode(Hardrectilinear *rect)
      : rect(rect), x(rect->x), y(rect->y), rotated(rect->rotated) {}
  int getWidth() const { return rect->getWidth(rotated); }
  int getHeight() const { return rect->getHeight(rotated); }
  Point center() const override {
    int xc = getWidth() / 2;
    int yc = getHeight() / 2;
    return Point(x + xc, y + yc);
  }
  void updateAns() {
    rect->x = x;
    rect->y = y;
    rect->rotated = rotated;
  }
};

struct LinkListNode : public BaseNode {
  LinkListNode *prev, *next;
  LinkListNode(Hardrectilinear *rect = nullptr)
      : BaseNode(rect), prev(nullptr), next(nullptr) {}
};

struct Node : public LinkListNode {
  Node *lc, *rc, **pa;
  Node(Hardrectilinear *rect)
      : LinkListNode(rect), lc(nullptr), rc(nullptr), pa(nullptr) {}
};