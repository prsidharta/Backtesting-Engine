#include "SimEngine.h"
#include <chrono>
#include <iostream>
#include <stdexcept>

SimEngine::SimEngine(double startingCash, TradingStrategy *strategy) {
    m_cash = startingCash;
    m_shares = 0;
    m_strategy = strategy;
}

double SimEngine::GetPortfolioValue(double currentPrice) {
    return m_cash + (m_shares * currentPrice);
}

void SimEngine::RunSimulator(const std::vector<double> &prices) {

    std::cout << "START CASH: " << m_cash << "\n";

    auto start = std::chrono::high_resolution_clock::now();

    double benchmarkShares = m_cash / prices[0];
    std::vector<TradingRecord> ledger;

    for (size_t day = 0; day < prices.size(); day++) {
        double dayPrice = prices[day];

        int signal = m_strategy->CreateSignal(dayPrice, m_shares);

        if (signal == 1) {
            m_shares = m_cash / dayPrice;
            m_cash -= (m_shares * dayPrice);
            ledger.emplace_back("BUY", dayPrice, m_shares);
            /*std::cout << "BUY+" << "\n"
                      << "Stock Price: " << dayPrice << "\n"
                      << "Shares bought: " << m_shares << "\n"
                      << "Current Cash: " << m_cash << "\n"; */
        } else if (signal == -1) {
            m_cash += (m_shares * dayPrice);
            double sharesSold = m_shares;
            m_shares = 0;
            ledger.emplace_back("SELL", dayPrice, sharesSold);
            /*std::cout << "SELL-" << "\n"
                      << "Stock Price: " << dayPrice << "\n"
                      << "Shares Sold: " << sharesSold << "\n"
                      << "Current Cash: " << m_cash << "\n";*/
        }
    }
    std::cout << "Buy and Hold End Cash: " << benchmarkShares * prices.back() << "\n";

    if (ledger.empty()) {
        std::cout << "No Trades Executed" << "\n";
    } else {
        int wins = 0;
        int losses = 0;
        size_t totalTrades = ledger.size() / 2;
        for (size_t i = 0; i < ledger.size() - 1; i += 2) {
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

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    std::cout << "END CASH: " << GetPortfolioValue(prices.back()) << "\n"
              << "Program Time: " << duration.count() << "ns" << std::endl;
}