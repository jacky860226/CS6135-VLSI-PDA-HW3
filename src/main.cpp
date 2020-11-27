#include "GlobalTimer.hpp"
#include "Parser.hpp"
#include "SimulatedAnnealing.hpp"
#include "TreeInitializer.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  // ./hw3 *.hardblocks *.nets *.pl *.floorplan dead_space_ratio
  GlobalTimer::setTimeLimit(std::chrono::seconds(60 * 15));

  if (argc <= 5) {
    std::cerr << "Argument Error!\n";
    return -1;
  }

  Parser parser;
  auto middle = parser.parse(argv[1], argv[2], argv[3], argv[5]);
  RandomTreeInitializer randomTreeInitializer(7122);
  auto b_star_tree = randomTreeInitializer.createTree(*middle);
  std::cout << "Start" << std::endl;

  N100_Cost n100_Cost;
  N300_Cost n300_Cost;

  double P = 1.1;
  double Eps = 1e-9;
  double R = 0.99;
  int K = 7;
  size_t seed = 7122;
  double delta_avg = 7122.0;
  double reject_rate = 1;
  CostFunctionBase *costFunction = nullptr;

  if (middle->hardblocks.size() <= 100) {
    costFunction = &n100_Cost;
  } else {
    costFunction = &n300_Cost;
    reject_rate = 0.997;
    if (middle->hardblocks.size() <= 200) {
      delta_avg = 3736.84;
      seed = 1;
      Eps = 0.000005;
    } else {
      reject_rate = 0.9974;
      delta_avg = 2217.2;
      seed = 4; // 4
      Eps = 0.001;
    }
  }

  SimulatedAnnealing SA(P, Eps, R, K, seed, costFunction);
  auto delta = SA.solve(b_star_tree, delta_avg, reject_rate);

  std::cout << "Delta AVG: " << delta << '\n';
  std::cout << "......" << middle->width_height << '\n';
  auto xy = middle->maxBondary();
  std::cout << ".." << xy.first << ' ' << xy.second << ' ' << middle->HPWL()
            << '\n';

  middle->output_to_file(argv[4]);

  if (GlobalTimer::overTime())
    std::cout << "Time!!!\n";
  return 0;
}