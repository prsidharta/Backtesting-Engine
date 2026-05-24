/**
 * @file main.cpp
 * @author Patrick Sidharta
 * @brief Beginning stage of backtesting engine. Currently handles mock initialization, mock values, and run simulation
 */

#include "SimEngine.h"
#include "StockParser.h"
#include <chrono>
#include <iostream>
#include <vector>

int main() {

    auto start = std::chrono::high_resolution_clock::now();
    double cash = 10000.00;
    double shares = 0.0;
    int windowSize = 3;
    std::vector<double> prices = ReadCsv("SPY.csv");

    std::cout << "START CASH: " << cash << std::endl;
    std::vector<double> windowAverages = CalculateWindowAverages(prices, windowSize);

    RunSimulator(prices, windowAverages, cash, shares, windowSize);

    cash = cash + (shares * prices.back());
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "END CASH: " << cash << "\n"
              << "Program Time: " << duration.count() << "ms" << std::endl;

    return 0;
}