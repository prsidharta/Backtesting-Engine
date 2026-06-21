#include "SimEngine.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <stdexcept>

#ifdef __linux__
#include <sys/resource.h>
#endif

SimEngine::SimEngine(double startingCash, TradingStrategy *strategy) {
    m_startingCash = startingCash;
    m_cash = startingCash;
    m_shares = 0.0;
    m_strategy = strategy;
}

double SimEngine::GetPortfolioValue(double currentPrice) const {
    return m_cash + (m_shares * currentPrice);
}

SimulationStats SimEngine::Simulate(const std::vector<double> &prices) {
    if (prices.empty()) {
        throw std::invalid_argument("SimEngine::Simulate: prices vector is empty");
    }

    // Reset variables to beginning -> allows for multiple program executions
    m_cash = m_startingCash;
    m_shares = 0.0;

    SimulationStats stats;
    std::vector<double> portfolioValues;
    portfolioValues.reserve(prices.size());

    double firstValidPrice = 0.0;
    double lastValidPrice = 0.0;

    auto start = std::chrono::high_resolution_clock::now();

    for (double dayPrice : prices) {
        // KEEP THIS -> DIVISION BY ZERO GUARD
        if (dayPrice <= 0.0) {
            continue;
        }

        if (firstValidPrice == 0.0) {
            firstValidPrice = dayPrice;
        }
        lastValidPrice = dayPrice;

        int signal = m_strategy->CreateSignal(dayPrice, m_shares);

        if (signal == 1 && m_shares == 0.0) {
            m_shares = m_cash / dayPrice;
            m_cash = 0.0;
            stats.ledger.emplace_back("BUY", dayPrice, m_shares);
        } else if (signal == -1 && m_shares > 0.0) {
            m_cash += m_shares * dayPrice;
            double sharesSold = m_shares;
            m_shares = 0.0;
            stats.ledger.emplace_back("SELL", dayPrice, sharesSold);
        }

        portfolioValues.push_back(GetPortfolioValue(dayPrice));
    }

    auto end = std::chrono::high_resolution_clock::now();
    stats.computeTimeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    if (firstValidPrice == 0.0) {
        throw std::runtime_error("SimEngine::Simulate: no valid (positive) prices in dataset");
    }

    double benchmarkShares = m_startingCash / firstValidPrice;
    stats.benchmarkEndValue = benchmarkShares * lastValidPrice;
    stats.endingPortfolioValue = GetPortfolioValue(lastValidPrice);
    stats.endingCash = m_cash;
    stats.roi = ((stats.endingPortfolioValue - m_startingCash) / m_startingCash) * 100.0;

    ComputeTradeStatistics(stats);
    ComputeRiskMetrics(stats, portfolioValues);
    ComputePerformanceMetrics(stats, prices.size());

    return stats;
}

void SimEngine::ComputeTradeStatistics(SimulationStats &stats) const {
    if (stats.ledger.size() < 2) {
        return;
    }

    stats.totalTrades = stats.ledger.size() / 2;

    // i incremented by 2 since each BUY is followed by SELL
    for (size_t i = 0; i + 1 < stats.ledger.size(); i += 2) {
        double tradePnl = stats.ledger[i + 1].price - stats.ledger[i].price;

        if (tradePnl > 0.0) {
            stats.wins++;
            stats.grossProfit += tradePnl;
            stats.largestWin = std::max(stats.largestWin, tradePnl);
        } else if (tradePnl < 0.0) {
            stats.losses++;
            stats.grossLoss += std::abs(tradePnl);
            stats.largestLoss = std::max(stats.largestLoss, std::abs(tradePnl));
        }
    }

    stats.winRate = stats.totalTrades > 0
                        ? (static_cast<double>(stats.wins) / static_cast<double>(stats.totalTrades)) * 100.0
                        : 0.0;
    stats.avgWin = stats.wins > 0 ? stats.grossProfit / stats.wins : 0.0;
    stats.avgLoss = stats.losses > 0 ? stats.grossLoss / stats.losses : 0.0;
    stats.profitFactor = stats.grossLoss > 0.0 ? stats.grossProfit / stats.grossLoss : stats.grossProfit;
}

void SimEngine::ComputeRiskMetrics(SimulationStats &stats, const std::vector<double> &portfolioValues) const {
    if (portfolioValues.empty()) {
        return;
    }

    // Max Drawdown -> largest peak to trough drop before new peak
    double peak = portfolioValues[0];
    double maxDrawdown = 0.0;
    for (double value : portfolioValues) {
        peak = std::max(peak, value);
        if (peak > 0.0) {
            maxDrawdown = std::max(maxDrawdown, (peak - value) / peak);
        }
    }
    stats.maxDrawdownPct = maxDrawdown * 100.0;

    if (portfolioValues.size() < 2) {
        return;
    }

    std::vector<double> dailyReturns;
    dailyReturns.reserve(portfolioValues.size() - 1);
    for (size_t i = 1; i < portfolioValues.size(); i++) {
        if (portfolioValues[i - 1] > 0.0) {
            dailyReturns.push_back((portfolioValues[i] - portfolioValues[i - 1]) / portfolioValues[i - 1]);
        }
    }

    if (dailyReturns.empty()) {
        return;
    }

    double meanReturn = std::accumulate(dailyReturns.begin(), dailyReturns.end(), 0.0) / dailyReturns.size();

    double sumSquaredDiff = 0.0;
    for (double r : dailyReturns) {
        sumSquaredDiff += (r - meanReturn) * (r - meanReturn);
    }
    double stdDev = std::sqrt(sumSquaredDiff / dailyReturns.size());

    // 252 is the number of trading days in a year -> annualize Sharpe Ratio
    stats.sharpeRatio = stdDev > 0.0 ? (meanReturn / stdDev) * std::sqrt(252.0) : 0.0;
}

void SimEngine::ComputePerformanceMetrics(SimulationStats &stats, size_t rowCount) const {
    double computeSeconds = stats.computeTimeNs / 1e9;
    stats.throughputRowsPerSec = computeSeconds > 0.0 ? static_cast<double>(rowCount) / computeSeconds : 0.0;

#ifdef __linux__
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    stats.peakMemoryMb = usage.ru_maxrss / 1024.0;
#endif
}

void SimEngine::PrintStats(const SimulationStats &stats) const {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Buy and Hold End Cash: " << stats.benchmarkEndValue << "\n";

    if (stats.ledger.empty()) {
        std::cout << "No Trades Executed\n";
    } else {
        std::cout << "\n--- Trade Simulation Stats ---\n"
                  << "Win rate: " << stats.winRate << "%\n"
                  << "Wins: " << stats.wins << "\n"
                  << "Losses: " << stats.losses << "\n"
                  << "Total Trades: " << stats.totalTrades
                  << "\n"
                  /* Trivial stats, uncomment later
                  << "Gross Profit (Per Share): $" << stats.grossProfit << "\n"
                  << "Gross Loss (Per Share): $" << stats.grossLoss << "\n"
                  << "Average Win (Per Share): $" << stats.avgWin << "\n"
                  << "Average Loss (Per Share): $" << stats.avgLoss << "\n"
                  << "Largest Win (Per Share): $" << stats.largestWin << "\n"
                  << "Largest Loss (Per Share): $" << stats.largestLoss << "\n"
                  */
                  << "Profit Factor: " << stats.profitFactor << "\n"
                  << "Total ROI: " << stats.roi << "%\n"
                  << "Max Drawdown: " << stats.maxDrawdownPct << "%\n"
                  << "Sharpe Ratio: " << stats.sharpeRatio << "\n";
    }

    std::cout << "\n--- Computer Performance Metrics ---\n"
              << "END CASH: $" << stats.endingPortfolioValue << "\n"
              << "Core Compute Time: " << stats.computeTimeNs << " ns\n"
              << "Engine Throughput: " << stats.throughputRowsPerSec << " rows/sec\n"
              << "Peak Memory Usage: " << stats.peakMemoryMb << " MB\n"
              << "---------------------------\n"
              << std::endl;
}

void SimEngine::RunSimulator(const std::vector<double> &prices) {
    std::cout << "START CASH: " << m_startingCash << "\n";
    SimulationStats stats = Simulate(prices);
    PrintStats(stats);
}
