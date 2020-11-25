#pragma once
#include "B_star_tree.hpp"
#include "Middle.hpp"
#include <random>

struct BaseTreeInitializer {
  virtual B_star_tree createTree(Middle &middle) = 0;
};

struct RandomTreeInitializer : public BaseTreeInitializer {
  std::vector<Node> *nodes;
  std::mt19937 MT;
  Node *dfs(int l, int r) {
    if (l > r)
      return nullptr;
    std::uniform_int_distribution<int> distribution(l, r);
    int mid = distribution(MT);
    auto res = &nodes->at(mid);
    res->lc = dfs(l, mid - 1);
    res->rc = dfs(mid + 1, r);
    return res;
  }
  B_star_tree createTree(Middle &middle) override {
    nodes = &middle.nodes;
    B_star_tree res;
    res.root = dfs(0, int(nodes->size()) - 1);
    return res;
  }
  RandomTreeInitializer(size_t seed = 7122) : MT(seed) {}
};