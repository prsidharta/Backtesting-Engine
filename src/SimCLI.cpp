#include "SimCLI.h"

#include "S_PPO.h"
#include "S_BollingerRSI.h"
#include "S_MovingAverage.h"
#include "SimEngine.h"
#include "StockParser.h"
#include "AWSConnection.h"

#include <cstdlib>
#include <exception>
#include <fstream>
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
              << "[3] PPO Model\n"
              << "> ";

    std::cin >> selectStrat;
}

void SimCLI::RunSim() {
    std::cout << "\nFetching historical data for " << selectStock << "...\n";

    //Old Python script data retrieval
    /*std::string command = "python3 scripts/getData.py " + selectStock;
    int fetchResult = std::system(command.c_str());
    if (fetchResult != 0){
        std::cout << "[!] ERROR! Failed to fetch " << selectStock << " data. Terminating program.\n";
        return;
    }
    */

    AWSConnection cloud_link;
    bool success = cloud_link.FetchData(selectStock);                     
    std::string filepath = "data/" + selectStock + ".csv";                 
    if (!success) {   
        if (!std::ifstream(filepath).is_open()) {
            std::cout << "\nFailed: Failed to securely fetch S3 data and no local copy exists. Exiting...\n";
            return;
        }
        std::cout << "\nS3 fetch failed, using local file " << filepath << "\n";
    }  
    
    std::vector<double> prices = ReadCsv(filepath);   
    double startCash = 10000.00;
    const double costRate = 0.0005;

    switch (selectStrat) {
    case 1: {
        S_MovingAverage strategy(3);
        SimEngine engine(startCash, &strategy, costRate);
        engine.RunSimulator(prices);
        break;
    }
    case 2: {
        S_BollingerRSI strategy(20, 2.0, 14, 70, 30);
        SimEngine engine(startCash, &strategy, costRate);
        engine.RunSimulator(prices);
        break;
    }
    case 3: {
        try {
            S_PPO strategy("models/PPO.txt");
            SimEngine engine(startCash, &strategy, costRate);
            engine.RunSimulator(prices);
        } catch (const std::exception &e) {
                std::cout << "Could not load PPO model: " << e.what() << "\n";
            }
        break;
    }
    default:
        std::cout << "Invalid strategy selected. Exiting...\n";
    }
}
