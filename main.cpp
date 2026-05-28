/**
 * @file main.cpp
 * @author Patrick Sidharta
 * @brief Beginning stage of backtesting engine. Currently handles mock initialization, mock values, and run simulation
 */

#include "S_MovingAverage.h"
#include "SimEngine.h"
#include "StockParser.h"
#include <vector>

int main() {
    double cash = 10000.00;
    int windowSize = 3;

    S_MovingAverage myStrategy(windowSize);
    SimEngine myEngine(cash, &myStrategy);

    std::vector<double> prices = ReadCsv("SPY.csv");

    myEngine.RunSimulator(prices);

    return 0;
}