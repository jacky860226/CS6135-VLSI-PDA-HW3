#pragma once
#include "Input.hpp"
#include "Nodes.hpp"
#include <cmath>
#include <unordered_map>
#include <utility>
#include <vector>

struct Middle {
  std::vector<Hardrectilinear> hardblocks;
  std::vector<Terminal> terminals;
  std::vector<Net> nets;
  std::vector<Node> nodes;
  std::unordered_map<const Hardrectilinear *, BaseNode *> nodeMap;
  int width_height;

  Middle(std::vector<Hardrectilinear> &&HB, std::vector<Terminal> &&TM,
         std::vector<Net> &&NT, double dead_space_ratio)
      : hardblocks(std::move(HB)), terminals(std::move(TM)),
        nets(std::move(NT)) {
    long long total_area = 0;
    for (const auto &hb : hardblocks)
      total_area += hb.width * hb.height;
    width_height = std::sqrt(total_area * (1 + dead_space_ratio));
    nodes.reserve(hardblocks.size());
    for (auto &hb : hardblocks)
      nodes.emplace_back(&hb);
    for (auto &nd : nodes)
      nodeMap[nd.rect] = &nd;
  }
  bool slow_verify() const {
    for (size_t i = 0; i < hardblocks.size(); ++i) {
      for (size_t j = i + 1; j < hardblocks.size(); ++j) {
        if (hardblocks[i].isIntersect(hardblocks[j]))
          return false;
      }
    }
    return true;
  }
  void updateAns() {
    for (auto &nd : nodes)
      nd.updateAns();
  }
  long long HPWL() const {
    long long ans = 0;
    for (const auto &nt : nets)
      ans += nt.HPWL();
    return ans;
  }
  long long HPWL_in_tree() const {
    long long ans = 0;
    for (const auto &nt : nets)
      ans += nt.HPWL_in_tree(nodeMap);
    return ans;
  }
  std::pair<long long, long long> maxBondary() const {
    long long maxW = 0, maxH = 0;
    for (const auto &hd : hardblocks) {
      maxW = std::max(maxW, (long long)(hd.x + hd.getWidth()));
      maxH = std::max(maxH, (long long)(hd.y + hd.getHeight()));
    }
    return std::make_pair(maxW, maxH);
  }
  std::pair<long long, long long> maxBondary_in_tree() const {
    long long maxW = 0, maxH = 0;
    for (const auto &nd : nodes) {
      maxW = std::max(maxW, (long long)(nd.x + nd.getWidth()));
      maxH = std::max(maxH, (long long)(nd.y + nd.getHeight()));
    }
    return std::make_pair(maxW, maxH);
  }
};