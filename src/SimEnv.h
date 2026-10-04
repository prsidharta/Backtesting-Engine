#pragma once

#include "FeatureBuilder.h"
#include <cstddef>
#include <vector>

/**
 * @struct StepResult
 * @brief What the environment returns after one decision
 */
struct StepResult {
    double reward;         ///< log(portfolio value next bar / portfolio value now), after trading costs
    bool done;             ///< True when the episode window is finished
    double portfolioValue; ///< Portfolio value at the new bar
};

/**
 * @class SimEnv
 * @brief Step-by-step view of the same simulation SimEngine runs in batch (for RL training)
 * @details Actions: 0 = want to be flat, 1 = want to be long. Trades execute through ApplySignal at the
 * current bar's price, then the clock advances one bar and the reward is the resulting change in value.
 */
class SimEnv {
  public:
    /// @throws std::invalid_argument if prices is too short or contains a non-positive value
    SimEnv(std::vector<double> prices, double startingCash, double costRate);

    /**
     * @brief Starts an episode covering bars [start, end)
     * @param start First bar the agent decides on (must be >= FeatureBuilder::kWarmup)
     * @param end One past the last bar of the episode (must be <= number of prices, > start + 1)
     * @return The first observation
     */
    std::vector<double> Reset(size_t start, size_t end);

    /// @brief Applies action (0 = flat, 1 = long) at the current bar, then advances one bar
    StepResult Step(int action);

    /// @brief Observation for the current bar
    std::vector<double> Observe() const;

    size_t NumBars() const { return m_prices.size(); }
    static int NumFeatures() { return FeatureBuilder::kNumFeatures; }
    static size_t Warmup() { return FeatureBuilder::kWarmup; }

  private:
    double Value(double price) const { return m_cash + m_shares * price; }

    std::vector<double> m_prices;
    double m_startingCash;
    double m_costRate;
    double m_cash = 0.0;
    double m_shares = 0.0;
    double m_entryPrice = 0.0;
    size_t m_t = 0;   ///< Current bar index
    size_t m_end = 0; ///< Exclusive end of the episode
    FeatureBuilder m_features;
};
