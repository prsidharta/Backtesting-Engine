#pragma once

#include <string>
#include <vector>

class SimCLI {
  private:
    int selectStrat;

    void DisplayStocksMenu();
    void DisplayStrategyMenu();
    void RunSim();

  public:
    SimCLI();
    void Run();
};