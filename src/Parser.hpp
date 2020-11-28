#pragma once
#include "Middle.hpp"
#include <cstdio>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>

struct Parser {
  std::vector<Hardrectilinear> hardblocks;
  std::vector<Terminal> terminals;
  std::vector<Net> nets;

  std::unordered_map<std::string, Hardrectilinear *> str2hd;
  std::unordered_map<std::string, Terminal *> str2ter;

  void readHardblocks(const std::string &hardblocksFile) {
    std::ifstream fin(hardblocksFile);
    std::string buff, name, type, trash, rem;
    while (std::getline(fin, buff)) {
      if (buff.empty())
        continue;
      std::stringstream ss(buff);
      ss >> name;
      if (name == "NumHardRectilinearBlocks" || name == "NumTerminals")
        continue;
      ss >> type;
      if (type != "hardrectilinear")
        continue;
      ss >> trash;
      std::getline(ss, rem);
      int x[4], y[4];
      sscanf(rem.c_str(), " (%d, %d) (%d, %d) (%d, %d) (%d, %d)", x, y, x + 1,
             y + 1, x + 2, y + 2, x + 3, y + 3);
      hardblocks.emplace_back(name);
      hardblocks.back().width =
          *std::max_element(x, x + 4) - *std::min_element(x, x + 4);
      hardblocks.back().height =
          *std::max_element(y, y + 4) - *std::min_element(y, y + 4);
    }
    for (auto &hb : hardblocks) {
      str2hd[hb.name] = &hb;
    }
  }
  void readNets(const std::string &netsFile) {
    std::string buff, name, type, trash, rem;
    std::ifstream fin(netsFile);
    while (std::getline(fin, buff)) {
      if (buff.empty())
        continue;
      std::stringstream ss(buff);
      ss >> name;
      if (name == "NumNets" || name == "NumPins")
        continue;
      if (name == "NetDegree") {
        int size;
        ss >> name >> size;
        nets.emplace_back();
        while (size--) {
          std::getline(fin, buff);
          if (str2hd.count(buff)) {
            nets.back().hardrectilinears.emplace_back(str2hd.at(buff));
          } else if (str2ter.count(buff)) {
            nets.back().terminals.emplace_back(str2ter.at(buff));
          } else {
            // error!
          }
        }
      } else {
        // error!
      }
    }
  }
  void readPL(const std::string &plFile) {
    std::ifstream fin(plFile);
    std::string name;
    int x, y;
    while (fin >> name >> x >> y) {
      terminals.emplace_back(name);
      terminals.back().x = x;
      terminals.back().y = y;
    }
    for (auto &ter : terminals) {
      str2ter[ter.name] = &ter;
    }
  }

  std::shared_ptr<Middle> parse(const std::string &hardblocksFile,
                                const std::string &netsFile,
                                const std::string &plFile,
                                const std::string &dead_space_ratio) {
    hardblocks.clear();
    terminals.clear();
    nets.clear();
    str2hd.clear();
    str2ter.clear();

    std::thread readPLThread(&Parser::readPL, this, plFile);
    std::thread readHardblocksThread(&Parser::readHardblocks, this,
                                     hardblocksFile);
    readPLThread.join();
    readHardblocksThread.join();
    readNets(netsFile);
    return std::make_shared<Middle>(std::move(hardblocks), std::move(terminals),
                                    std::move(nets),
                                    std::stod(dead_space_ratio));
  }
};