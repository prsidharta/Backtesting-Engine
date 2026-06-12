#include "SimCLI.h"
#include "S_BollingerRSI.h"
#include "S_MovingAverage.h"
#include "SimEngine.h"
#include "StockParser.h"

#include <cstdlib>
#include <iostream>

SimCLI::SimCLI() : selectStock("SPY"), selectStrat(1) {
}

void SimCLI::Run() {
    std::cout << "\nEngine running." << "\n";
    DisplayStocksMenu();
    DisplayStrategyMenu();
    RunSim();
}

void SimCLI::DisplayStocksMenu() {
    std::cout << "\nSelect asset: \n"
              << "[1] SPY\n"
              << "[2] TSLA\n"
              << "[3] AAPL\n"
              << "[4] VOO\n"
              << "> ";

    int choice;
    std::cin >> choice;

    switch (choice) {
    case 2:
        selectStock = "TSLA";
        break;
    case 3:
        selectStock = "AAPL";
        break;
    case 4:
        selectStock = "VOO";
        break;
    default:
        selectStock = "SPY";
        break;
    }
}

void SimCLI::DisplayStrategyMenu() {
    std::cout << "\nSelect Simulation Strategy:\n"
              << "[1] Simple Moving Average\n"
              << "[2] Bollinger Bands + RSI\n"
              << "> ";

    std::cin >> selectStrat;
}

void SimCLI::RunSim() {
    std::cout << "\nFetching historical data for " << selectStock << "...\n";

    std::string command = "python3 scripts/getData.py " + selectStock;

    int fetchResult = std::system(command.c_str());

    if (fetchResult != 0){
        std::cout << "[!] ERROR! Failed to fetch " << selectStock << " data. Terminating program.\n";
        return;
    }

    std::string filepath = "data/" + selectStock + ".csv";
    std::vector<double> prices = ReadCsv(filepath);

    double startCash = 10000.00;

    switch (selectStrat) {
    case 1: {
        S_MovingAverage strategy(3);
        SimEngine engine(startCash, &strategy);
        engine.RunSimulator(prices);
        break;
    }
    case 2: {
        S_BollingerRSI strategy(20, 2.0, 14, 70, 30);
        SimEngine engine(startCash, &strategy);
        engine.RunSimulator(prices);
        break;
    }
    default:
        std::cout << "[!] Invalid strategy selected. Terminating program.\n";
    }
}