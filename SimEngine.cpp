#include "SimEngine.h"

#include <iostream>
#include <stdexcept>

std::vector<double> CalculateWindowAverages(std::vector<double> &prices, int windowSize) {

    if (prices.size() == 0) {
        throw std::invalid_argument("Empty Vector");
    }

    if (windowSize > prices.size()) {
        throw std::invalid_argument("Requested Window Period is Greater than Vector Provided");
    }

    std::vector<double> windowAverages;

    for (int i = 0; i < prices.size() - (windowSize - 1); i++) {
        double temp = 0;
        for (int j = 0; j < windowSize; j++) {
            temp += prices[i + j];
        }

        temp = temp / windowSize;
        windowAverages.push_back(temp);
    }

    return windowAverages;
}

void RunSimulator(std::vector<double> &prices, std::vector<double> &windowAverages, double &cash, double &shares,
                  int windowSize) {

    double benchmarkShares = cash / prices[0];
    std::vector<TradingRecord> ledger;

    for (size_t day = 0; day < windowAverages.size(); day++) {
        int dayOffset = day + (windowSize - 1);
        double dayPrice = prices[dayOffset];
        double windowPrice = windowAverages[day];

        if (shares == 0 && dayPrice > windowPrice) {
            shares = cash / dayPrice;
            cash -= (shares * dayPrice);
            ledger.emplace_back("BUY", dayPrice, shares);
            std::cout << "BUY+" << "\n"
                      << "Stock Price: " << dayPrice << "\n"
                      << "Moving Average: " << windowPrice << "\n"
                      << "Shares bought: " << shares << "\n"
                      << "Current Cash: " << cash << "\n";
        } else if (shares != 0 && dayPrice < windowPrice) {
            cash += (shares * dayPrice);
            double sharesSold = shares;
            shares = 0;
            ledger.emplace_back("SELL", dayPrice, sharesSold);
            std::cout << "SELL-" << "\n"
                      << "Stock Price: " << dayPrice << "\n"
                      << "Moving Average: " << windowPrice << "\n"
                      << "Shares Sold: " << sharesSold << "\n"
                      << "Current Cash: " << cash << "\n";
        }
    }
    std::cout << "Buy and Hold End Cash: " << benchmarkShares * prices.back() << "\n";

    if (ledger.empty()){
        std::cout << "No Trades Executed" << "\n";
    }
    else{
        int wins = 0;
        int losses = 0;
        int totalTrades = ledger.size() / 2;
        for (int i = 0; i < ledger.size() - 1; i += 2){
            if (ledger[i + 1].price > ledger[i].price){
                wins++;
            }
            else {
                losses++;
            }
        }
        std::cout << "Win rate: " << ((double)wins / totalTrades) * 100.0 << "\n"
        << "Wins: " << wins << "\n"
        << "Losses: " << losses << "\n"
        << "Total Trades: " << totalTrades << "\n";
    }
}