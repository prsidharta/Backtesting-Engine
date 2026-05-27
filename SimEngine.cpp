#include "SimEngine.h"
#include <iostream>
#include <stdexcept>

SimEngine::SimEngine(double startingCash, TradingStrategy *strategy) {
    m_cash = startingCash;
    m_shares = 0;
    m_strategy = strategy;
}

double SimEngine::GetCash() {
    return m_cash;
}

double SimEngine::GetShares() {
    return m_shares;
}

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

void SimEngine::RunSimulator(std::vector<double> &prices, std::vector<double> &windowAverages, int windowSize) {

    double benchmarkShares = m_cash / prices[0];
    std::vector<TradingRecord> ledger;

    for (size_t day = 0; day < windowAverages.size(); day++) {
        int dayOffset = day + (windowSize - 1);
        double dayPrice = prices[dayOffset];
        double windowPrice = windowAverages[day];

        int signal = m_strategy->CreateSignal(dayPrice, windowPrice, m_shares);

        if (signal == 1) {
            m_shares = m_cash / dayPrice;
            m_cash -= (m_shares * dayPrice);
            ledger.emplace_back("BUY", dayPrice, m_shares);
            std::cout << "BUY+" << "\n"
                      << "Stock Price: " << dayPrice << "\n"
                      << "Moving Average: " << windowPrice << "\n"
                      << "Shares bought: " << m_shares << "\n"
                      << "Current Cash: " << m_cash << "\n";
        } else if (signal == -1) {
            m_cash += (m_shares * dayPrice);
            double sharesSold = m_shares;
            m_shares = 0;
            ledger.emplace_back("SELL", dayPrice, sharesSold);
            std::cout << "SELL-" << "\n"
                      << "Stock Price: " << dayPrice << "\n"
                      << "Moving Average: " << windowPrice << "\n"
                      << "Shares Sold: " << sharesSold << "\n"
                      << "Current Cash: " << m_cash << "\n";
        }
    }
    std::cout << "Buy and Hold End Cash: " << benchmarkShares * prices.back() << "\n";

    if (ledger.empty()) {
        std::cout << "No Trades Executed" << "\n";
    } else {
        int wins = 0;
        int losses = 0;
        int totalTrades = ledger.size() / 2;
        for (int i = 0; i < ledger.size() - 1; i += 2) {
            if (ledger[i + 1].price > ledger[i].price) {
                wins++;
            } else {
                losses++;
            }
        }
        std::cout << "Win rate: " << ((double)wins / totalTrades) * 100.0 << "\n"
                  << "Wins: " << wins << "\n"
                  << "Losses: " << losses << "\n"
                  << "Total Trades: " << totalTrades << "\n";
    }
}