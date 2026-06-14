
#pragma once

#include <string>
#include <vector>

/**
 * @class SimCLI
 * @brief Handles specific execution logic based on user input
 */
class SimCLI {
  private:
    std::string selectStock;
    int selectStrat;

    /**
     * @brief Displays available stock options for the user, and saves user requested stock
     */
    void DisplayStocksMenu();

    /**
     * @brief Displays available trading strategies (Moving Average or Bollinger + RSI) and saves user requested strat
     */
    void DisplayStrategyMenu();

    void RunSim();

  public:
    SimCLI();
    void Run();
};