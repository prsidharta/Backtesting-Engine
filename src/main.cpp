/**
 * @file main.cpp
 * @author Patrick Sidharta
 * @brief Intermediate stage of backtesting engine. 
 * Currently has two strategies (Moving Average and RSI + Bollinger Bands) implemented.
 * Need to create user interface and allow for seamless transition between strategies as well as user implemented variables.
 */

#include "S_MovingAverage.h"
#include "S_BollingerRSI.h"
#include "SimEngine.h"
#include "StockParser.h"
#include <exception>
#include <iostream>
#include <vector>

int main() {
    //try {
        double cash = 10000.00;
        int windowSize = 3;

        // S_MovingAverage myStrategy(windowSize);
        
        S_BollingerRSI myStrategy(20, 2.0, 14, 70, 30);
        SimEngine myEngine(cash, &myStrategy);

        std::vector<double> prices = ReadCsv("data/SPY.csv");

        myEngine.RunSimulator(prices);
    /*} catch (const std::exception &e) {
        std::cerr << "\nFATAL CRASH TRAPPED: " << e.what() << "\n\n";
    }*/
    return 0;
}