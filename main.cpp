/**
 * @file main.cpp
 * @author Patrick Sidharta
 * @brief Beginning stage of backtesting engine. Currently handles mock initialization, mock values, and run simulation
 */

#include "SimEngine.h"
#include "StockParser.h"
#include <iostream>
#include <vector>

int main() {

    double cash = 10000.00;
    double shares = 0.0;
    int windowSize = 3;
    std::vector<double> prices = ReadCsv("SPY.csv");

    std::cout << "START CASH: " << cash << std::endl;
    std::vector<double> windowAverages = CalculateWindowAverages(prices, windowSize);

    RunSimulator(prices, windowAverages, cash, shares, windowSize);

    cash = cash + (shares * prices.back());
    std::cout << "END CASH: " << cash << std::endl;

    return 0;
}