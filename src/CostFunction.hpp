#pragma once
#include "Middle.hpp"

struct CostFunctionBase {
  std::pair<int, int> outline;
  int HPWL;
  virtual double operator()(Middle &middle) = 0;
  bool inRange(Middle &middle) const {
    return middle.width_height >= outline.first &&
           middle.width_height >= outline.second;
  }
  void calCurCost(Middle &middle) {
    outline = middle.maxBondary();
    HPWL = middle.HPWL();
  }
};

struct N100_Cost : public CostFunctionBase {
  double operator()(Middle &middle) {
    double alpha = 0.9 * 1000, beta = 0.2;
    outline = middle.maxBondary_in_tree();
    HPWL = middle.HPWL_in_tree();
    double widthCost = std::max(outline.first - middle.width_height, 0);
    double heightCost = std::max(outline.second - middle.width_height, 0);
    return alpha * (widthCost + heightCost) + beta * HPWL;
  }
};

struct N300_Cost : public CostFunctionBase {
  double operator()(Middle &middle) {
    double alpha = 0.9 * 1000, beta = 0.2;
    outline = middle.maxBondary_in_tree();
    HPWL = middle.HPWL_in_tree();
    double widthCost = std::max(outline.first - middle.width_height, 0);
    double heightCost = std::max(outline.second - middle.width_height, 0);
    return alpha * ((widthCost + 1) * (heightCost + 1)) + beta * HPWL;
  }
};