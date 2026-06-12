#pragma once

#include <string>
#include <vector>

class SimCLI {
  private:
    std::string selectStock;
    int selectStrat;

    void DisplayStocksMenu();
    void DisplayStrategyMenu();
    void RunSim();

  public:
    SimCLI();
    void Run();
};