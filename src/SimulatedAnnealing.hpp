#pragma once
#include "B_star_tree.hpp"
#include "CostFunction.hpp"
#include "GlobalTimer.hpp"
#include "Middle.hpp"
#include <cmath>
#include <limits>
#include <random>

#include <stdio.h>

struct SimulatedAnnealing {
  const double P;
  const double Eps;
  const double R;
  const int K;

  std::mt19937 randomGen;
  CostFunctionBase *costFunc;

  SimulatedAnnealing(double P, double Eps, double R, int K, size_t seed,
                     CostFunctionBase *costFunc)
      : P(P), Eps(Eps), R(R), K(K), randomGen(seed), costFunc(costFunc) {}

  BaseOperatorLog *randomMove(B_star_tree tree, size_t SwapSubtreeVal,
                              size_t SwapNodeVal, size_t RotateNodeVal) {
    size_t total =
        std::max(SwapSubtreeVal + SwapNodeVal + RotateNodeVal, size_t(1));
    std::uniform_int_distribution<size_t> moveVal(0, total - 1);
    size_t val = moveVal(randomGen);
    if (val < SwapSubtreeVal) {
      const auto &posArr = tree.getPosition();
      auto a = posArr.at(randomGen() % posArr.size());
      tree.getPosition(a, true);
      auto b = posArr.at(randomGen() % posArr.size());
      tree.swap_subtree(a, b);
      return new SwapSubtreeLog(tree, a, b);
    } else if (val < SwapSubtreeVal + SwapNodeVal) {
      size_t aid = randomGen() % tree.middle->nodes.size();
      size_t bid = randomGen() % tree.middle->nodes.size();
      if (aid == bid)
        bid = (bid + 1) % tree.middle->nodes.size();
      tree.swap_node(&tree.middle->nodes.at(aid), &tree.middle->nodes.at(bid));
      return new SwapNodeLog(tree, &tree.middle->nodes.at(aid),
                             &tree.middle->nodes.at(bid));
    } else {
      auto a = &tree.middle->nodes.at(randomGen() % tree.middle->nodes.size());
      tree.rotate_node(a);
      return new RotateNodeLog(tree, a);
    }
  }

  double solve(B_star_tree tree, double delta_avg, double reject_rate) {
    int N = tree.middle->nodes.size() * K;
    int MT, uphill, reject;
    double T = delta_avg / std::log(P);
    double cur_cost = (*costFunc)(*tree.middle);
    int best_cost = std::numeric_limits<int>::max();
    costFunc->calCurCost(*tree.middle);
    if (costFunc->inRange(*tree.middle))
      best_cost = costFunc->HPWL;
    std::uniform_real_distribution<double> distribution(0, 1);
    double delta_avg_cal = 0;
    int total_uphill = 0;
    do {
      MT = 0;
      uphill = 0;
      reject = 0;
      do {
        BaseOperatorLog *operatorLog = randomMove(tree, 1, 1, 1);
        tree.traceRectPosition();
        ++MT;
        double next_cost = (*costFunc)(*tree.middle);
        double delta_cost = next_cost - cur_cost;
        if (delta_cost <= 0 ||
            distribution(randomGen) < std::exp(-delta_cost / T)) {
          if (delta_cost > 0) {
            ++uphill;
            ++total_uphill;
            delta_avg_cal += delta_cost;
          }
          cur_cost = next_cost;
          if (costFunc->inRange(*tree.middle) && costFunc->HPWL < best_cost) {
            best_cost = costFunc->HPWL;
            tree.middle->updateAns();
          }
        } else {
          ++reject;
          operatorLog->undo();
        }
        delete operatorLog;
      } while (!(uphill > N || MT > 2 * N));
      T = R * T;
    } while (
        !(reject >= MT * reject_rate || T < Eps || GlobalTimer::overTime()));
    return delta_avg_cal / total_uphill;
  }
};