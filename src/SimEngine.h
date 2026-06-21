#pragma once

#include "TradingStrategy.h"
#include <string>
#include <vector>

/**
 * @struct TradingRecord
 * @brief Data container that holds all necessary information for one singular trade, used in vector of structs to
 * compile whole trading record of simulation
 */
struct TradingRecord {

    std::string type; ///< The transaction classification ("BUY", "HOLD", "SELL")
    double price;     ///< The price per share at time of trade
    double shares;    ///< The amount of shares bought or sold at the time of the trade

    /**
     * @brief Constructs a new TradingRecord instance
     * @param givenType The buy, hold, or sell signal
     * @param givenPrice The price of the shares at the time of the trade
     * @param givenShares The amount of shares bought or sold at the time of the trade
     */
    TradingRecord(const std::string givenType, double givenPrice, double givenShares) {
        type = givenType;
        price = givenPrice;
        shares = givenShares;
    }
};

struct SimulationStats {

    double endingCash = 0.0;
    double endingPortfolioValue = 0.0;
    double benchmarkEndValue = 0.0;
    double roi = 0.0;

    std::vector<TradingRecord> ledger;
    size_t totalTrades = 0;
    int wins = 0;
    int losses = 0;
    double winRate = 0.0;
    double grossProfit = 0.0;
    double grossLoss = 0.0;
    double avgWin = 0.0;
    double avgLoss = 0.0;
    double largestWin = 0.0;
    double largestLoss = 0.0;
    double profitFactor = 0.0;

    double maxDrawdownPct = 0.0;
    double sharpeRatio = 0.0;

    long long computeTimeNs = 0;
    double throughputRowsPerSec = 0.0;
    double peakMemoryMb = 0.0;
};

/**
 * @class SimEngine
 * @brief Execution engine that maintains portfolio state and executes trades based on the provided trading strategy
 */
class SimEngine {
  private:
    double m_startingCash;       ///< Initial balance the engine was constructed with; used to reset state between runs
    double m_cash;                ///< The current amount of cash ($USD) held by the portfolio
    double m_shares;              ///< Present held volume of equity
    TradingStrategy *m_strategy;  ///< Pointer to the trading logic that will be used in the simulation

    /**
     * @brief Computes win/loss counts, profit factor, and related trade-level statistics from the ledger
     * @param stats The in-progress simulation result to populate
     */
    void ComputeTradeStatistics(SimulationStats &stats) const;

    /**
     * @brief Computes max drawdown and annualized Sharpe ratio from the portfolio value history
     * @param stats The in-progress simulation result to populate
     * @param portfolioValues Portfolio value recorded at each simulated bar
     */
    void ComputeRiskMetrics(SimulationStats &stats, const std::vector<double> &portfolioValues) const;

    /**
     * @brief Computes throughput and peak memory usage for the completed simulation run
     * @param stats The in-progress simulation result to populate
     * @param rowCount Number of price rows processed
     */
    void ComputePerformanceMetrics(SimulationStats &stats, size_t rowCount) const;

    /**
     * @brief Prints a formatted summary of a completed simulation run to stdout
     * @param stats The completed simulation result to print
     */
    void PrintStats(const SimulationStats &stats) const;

  public:
    /**
     * @brief Constructs a new simulation engine with initial capital and a strategy to be tested
     * @param startingCash The initial balance in USD
     * @param strategy A pointer to the trading strategy to be utilized
     */
    SimEngine(double startingCash, TradingStrategy *strategy);

    /**
     * @brief Executes the trading strategy across the given price series and returns a structured result
     *
     * Resets internal cash/share state at the start of each call, so the same SimEngine instance can be
     * safely re-run multiple times (e.g. for benchmark timing) without state leaking between runs.
     *
     * @param prices A vector of historical stock prices
     * @return A SimulationStats struct containing portfolio outcome, trade stats, risk metrics, and timing
     * @throws std::invalid_argument if prices is empty
     * @throws std::runtime_error if prices contains no valid (positive) values
     */
    SimulationStats Simulate(const std::vector<double> &prices);

    /**
     * @brief Executes the trading strategy across the given price dates and prints a formatted summary
     * @param prices A vector of historical stock prices
     */
    void RunSimulator(const std::vector<double> &prices);

    /**
     * @brief Calculates total portfolio value at the given market price
     * @param currentPrice The current market price of the stock
     * @return The total value of the portfolio
     */
    double GetPortfolioValue(double currentPrice) const;
};
