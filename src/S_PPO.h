#pragma once

#include "FeatureBuilder.h"
#include "TradingStrategy.h"
#include <string>
#include <vector>

/**
 * @class MlpPolicy
 * @brief Tiny feed-forward network (tanh hidden layers, linear output) loaded from a text file
 * @details File format, written by python/export_policy.py:
 *   line 1: number of layers
 *   per layer: "<in> <out>", then out*in weights (row-major, PyTorch layout), then out biases
 */
class MlpPolicy {
  public:
    /// @throws std::runtime_error if the file is missing or malformed
    explicit MlpPolicy(const std::string &path);

    /// @brief Runs the network and returns the index of the largest output (ties -> lowest index)
    int Act(const std::vector<double> &obs) const;

    int InputSize() const { return m_layers.front().in; }

  private:
    struct Layer {
        int in = 0;
        int out = 0;
        std::vector<double> w;
        std::vector<double> b;
    };
    std::vector<Layer> m_layers;
};

/**
 * @class S_PPO
 * @brief Plugs a trained PPO policy into the engine as an ordinary TradingStrategy
 * @details Action 1 = want to be long, 0 = want to be flat, matching SimEnv.
 */
class S_PPO : public TradingStrategy {
  public:
    explicit S_PPO(const std::string &modelPath);

    int CreateSignal(double dayPrice, double currentShares) override;
    void Reset() override;

  private:
    MlpPolicy m_policy;
    FeatureBuilder m_features;
    double m_entryPrice = 0.0;
    double m_lastPrice = 0.0;
};
