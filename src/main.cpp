#include "GlobalTimer.hpp"
#include "Parser.hpp"
#include "SimulatedAnnealing.hpp"
#include "TreeInitializer.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  // ./hw3 *.hardblocks *.nets *.pl *.floorplan dead_space_ratio
  GlobalTimer::setTimeLimit(std::chrono::seconds(60 * 18));

  if (argc <= 5) {
    std::cerr << "Argument Error!\n";
    return -1;
  }

  bool multi_thread_read = true;
  if (argc > 6) {
    multi_thread_read = std::atoi(argv[6]);
  }

  Parser parser;
  auto middle =
      parser.parse(argv[1], argv[2], argv[3], argv[5], multi_thread_read);
  auto inputDuration = GlobalTimer::getDuration();

  RandomTreeInitializer randomTreeInitializer(7122);
  auto b_star_tree = randomTreeInitializer.createTree(*middle);
  auto buildTreeDuration = GlobalTimer::getDuration();

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
      seed = 4;
      Eps = 0.001;
    }
  }

  SimulatedAnnealing SA(P, Eps, R, K, seed, costFunction);
  auto delta = SA.solve(b_star_tree, delta_avg, reject_rate);
  auto SADuration = GlobalTimer::getDuration();

  std::cout << "Delta AVG: " << delta << '\n';
  std::cout << "......" << middle->width_height << '\n';
  auto xy = middle->maxBondary();
  std::cout << ".." << xy.first << ' ' << xy.second << ' ' << middle->HPWL()
            << '\n';

  middle->output_to_file(argv[4]);
  auto outputDuration = GlobalTimer::getDuration();

  auto inputTime = inputDuration.count();
  auto buildTreeTime = (buildTreeDuration - inputDuration).count();
  auto SATime = (SADuration - buildTreeDuration).count();
  auto outputTime = (outputDuration - SADuration).count();

  std::cout << "inputTime: " << inputTime / 1e9 << '\n';
  std::cout << "buildTreeTime: " << buildTreeTime / 1e9 << '\n';
  std::cout << "SATime: " << SATime / 1e9 << '\n';
  std::cout << "outputTime: " << outputTime / 1e9 << '\n';
  std::cout << "totalTime: " << outputDuration.count() / 1e9 << '\n';

  if (GlobalTimer::overTime())
    std::cout << "overTime!!!\n";
  std::cout << "\n";
  return 0;
}