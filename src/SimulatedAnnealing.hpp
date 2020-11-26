#pragma once
#include "B_star_tree.hpp"
#include "CostFunction.hpp"
#include "GlobalTimer.hpp"
#include "Middle.hpp"
#include <cmath>
#include <random>

struct SimulatedAnnealing {
  const double P;
  const double Eps;
  const double R;
  const int K;

  std::mt19937 randomGen;

  SimulatedAnnealing(double P, double Eps, double R, int K, size_t seed)
      : P(P), Eps(Eps), R(R), K(K), randomGen(seed) {}

  BaseOperatorLog *randomMove(B_star_tree tree, size_t SwapSubtreeVal,
                              size_t SwapNodeVal, size_t RotateNodeVal) {
    size_t total =
        std::max(SwapSubtreeVal + SwapNodeVal + RotateNodeVal, size_t(1));
    std::uniform_int_distribution<size_t> moveVal(0, total - 1);
    size_t val = moveVal(randomGen);
    if (val < SwapSubtreeVal) {
      auto posArr = tree.getPosition();
      auto a = posArr.at(randomGen() % posArr.size());
      posArr = tree.getPosition(a, true);
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

  void solve(B_star_tree tree, double delta_avg) {
    int N = tree.middle->nodes.size() * K;
    int MT, uphill, reject;
    double T = delta_avg / std::log(P);
    double cur_cost = CostFunction::CostInClass(*tree.middle);
    double best_cost = cur_cost;
    std::uniform_real_distribution<double> distribution(0, 1);
    do {
      MT = 0;
      uphill = 0;
      reject = 0;
      do {
        BaseOperatorLog *operatorLog = randomMove(tree, 1, 1, 1);
        tree.traceRectPosition();
        ++MT;
        double next_cost = CostFunction::CostInClass(*tree.middle);
        double delta_cost = next_cost - cur_cost;
        if (delta_cost <= 0 ||
            distribution(randomGen) < std::exp(-delta_cost / T)) {
          if (delta_cost > 0)
            ++uphill;
          cur_cost = next_cost;
          if (cur_cost < best_cost) {
            best_cost = cur_cost;
            tree.middle->updateAns();
          }
        } else {
          ++reject;
          operatorLog->undo();
          delete operatorLog;
        }
      } while (!(uphill > N || MT > 2 * N));
      T = R * T;
    } while (!(reject > MT * 0.95 || T < Eps || GlobalTimer::overTime()));
  }
};