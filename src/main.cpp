#include "GlobalTimer.hpp"
#include "Parser.hpp"
#include "TreeInitializer.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  // ./hw3 *.hardblocks *.nets *.pl *.floorplan dead_space_ratio
  GlobalTimer::setTimeLimit(std::chrono::seconds(60 * 15));

  Parser parser;
  auto middle = parser.parse(argv[1], argv[2], argv[3], argv[5]);
  RandomTreeInitializer randomTreeInitializer(7122);
  auto b_star_tree = randomTreeInitializer.createTree(*middle);
  b_star_tree.traceRectPosition();
  middle->updateAns();
  std::cout << "......" << middle->width_height << '\n';
  auto xy = middle->maxBondary();
  std::cout << ".." << xy.first << ' ' << xy.second << '\n';

  std::cout << "---------" << middle->slow_verify() << '\n';

  if (GlobalTimer::overTime())
    return 0;
  return 0;
}