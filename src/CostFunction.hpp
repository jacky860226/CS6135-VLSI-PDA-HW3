#pragma once
#include "Middle.hpp"

namespace CostFunction {
double CostInClass(Middle &middle) {
  double alpha = 0.9 * 1000, beta = 0.1;
  auto outline = middle.maxBondary_in_tree();
  double widthCost = std::max(outline.first - middle.width_height, 0LL);
  double heightCost = std::max(outline.second - middle.width_height, 0LL);
  return alpha * ((widthCost + 1) * (heightCost + 1)) +
         beta * middle.HPWL_in_tree();
}
}; // namespace CostFunction