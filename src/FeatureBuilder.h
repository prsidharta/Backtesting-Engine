#pragma once

#include <deque>
#include <vector>

/**
 * @class FeatureBuilder
 * @brief Turns a stream of prices into the observation vector a learned policy sees
 * @details Used by BOTH the training environment (SimEnv) and the deployed S_PPO so the
 * model sees identical inputs in training and in the backtester. Only uses prices up to and
 * including the latest one, so there is no look-ahead.
 */
class FeatureBuilder {
  public:
    static constexpr int kNumFeatures = 8;
    static constexpr size_t kWarmup = 20; ///< Prices needed *before* the current one for features to be valid

    FeatureBuilder() = default;

    /// @brief Forget all history
    void Reset();

    /// @brief Feed the next price (call once per bar, in order)
    void Update(double price);

    /// @brief True once enough history exists for every feature
    bool Ready() const;

    /**
     * @brief Builds the observation for the latest price
     * @param position 1.0 if currently holding shares, else 0.0
     * @param unrealizedPct Open profit as a fraction of entry price (0 when flat)
     * @return kNumFeatures values, each roughly unit-scale and clipped to [-5, 5]
     */
    std::vector<double> Features(double position, double unrealizedPct) const;

  private:
    std::deque<double> m_prices; ///< Most recent kWarmup + 1 prices
};
