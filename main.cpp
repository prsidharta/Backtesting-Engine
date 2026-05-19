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

    std::vector<double> prices;

    for (int i = 0; i < 5; i++) {
        prices.push_back(100 + (i * 5));
    }
    prices.push_back(80);
    prices.push_back(25);
    prices.push_back(10);
    prices.push_back(100);

    double cash = 10000.00;
    double shares = 0.0;
    int windowSize = 3;

    std::cout << "START CASH: " << cash << std::endl;

    std::vector<double> windowAverages = CalculateWindowAverages(prices, windowSize);

    RunSimulator(prices, windowAverages, cash, shares, windowSize);

    cash = cash + (shares * prices.back());
    std::cout << "END CASH: " << cash << std::endl;

    return 0;
}