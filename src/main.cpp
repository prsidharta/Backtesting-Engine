/**
 * @file main.cpp
 * @author Patrick Sidharta
 * @brief Intermediate stage of backtesting engine.
 * Currently has two strategies (Moving Average and RSI + Bollinger Bands) implemented.
 * Need to connect program to online buckets for data retrieval
 */

#include "SimCLI.h"
#include <exception>
#include <iostream>

int main() {
    try {
        SimCLI app;
        app.Run();
    } catch (const std::exception &err) {
        std::cerr << "Caught Error: " << err.what() << "\n";
        return 1;
    }
    return 0;
}