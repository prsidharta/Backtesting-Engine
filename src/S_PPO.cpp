#include "S_PPO.h"
#include <cmath>
#include <fstream>
#include <stdexcept>

MlpPolicy::MlpPolicy(const std::string &path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("MlpPolicy: cannot open model file: " + path);
    }
    int numLayers = 0;
    if (!(in >> numLayers) || numLayers < 1) {
        throw std::runtime_error("MlpPolicy: bad layer count");
    }
    for (int l = 0; l < numLayers; l++) {
        Layer layer;
        if (!(in >> layer.in >> layer.out) || layer.in < 1 || layer.out < 1) {
            throw std::runtime_error("MlpPolicy: bad layer header");
        }
        layer.w.resize(static_cast<size_t>(layer.in) * layer.out);
        layer.b.resize(layer.out);
        for (double &x : layer.w) {
            if (!(in >> x)) throw std::runtime_error("MlpPolicy: truncated weights");
        }
        for (double &x : layer.b) {
            if (!(in >> x)) throw std::runtime_error("MlpPolicy: truncated biases");
        }
        if (!m_layers.empty() && m_layers.back().out != layer.in) {
            throw std::runtime_error("MlpPolicy: layer sizes do not chain");
        }
        m_layers.push_back(std::move(layer));
    }
}

int MlpPolicy::Act(const std::vector<double> &obs) const {
    std::vector<double> x = obs;
    for (size_t l = 0; l < m_layers.size(); l++) {
        const Layer &layer = m_layers[l];
        std::vector<double> y(layer.out);
        for (int o = 0; o < layer.out; o++) {
            double acc = layer.b[o];
            for (int i = 0; i < layer.in; i++) {
                acc += layer.w[static_cast<size_t>(o) * layer.in + i] * x[i];
            }
            y[o] = (l + 1 < m_layers.size()) ? std::tanh(acc) : acc;
        }
        x = std::move(y);
    }
    int best = 0;
    for (size_t i = 1; i < x.size(); i++) {
        if (x[i] > x[best]) best = static_cast<int>(i);
    }
    return best;
}

S_PPO::S_PPO(const std::string &modelPath) : m_policy(modelPath) {
    if (m_policy.InputSize() != FeatureBuilder::kNumFeatures) {
        throw std::runtime_error("S_PPO: model expects a different number of features");
    }
}

void S_PPO::Reset() {
    m_features.Reset();
    m_entryPrice = 0.0;
    m_lastPrice = 0.0;
}

int S_PPO::CreateSignal(double dayPrice, double currentShares) {
    // The engine fills at the price of the bar where we signalled, so the entry price is the previous bar's price
    if (currentShares > 0.0 && m_entryPrice == 0.0) {
        m_entryPrice = m_lastPrice;
    } else if (currentShares == 0.0) {
        m_entryPrice = 0.0;
    }
    m_lastPrice = dayPrice;

    m_features.Update(dayPrice);
    if (!m_features.Ready()) {
        return 0;
    }

    const double unrealized = (currentShares > 0.0 && m_entryPrice > 0.0) ? dayPrice / m_entryPrice - 1.0 : 0.0;
    const int action = m_policy.Act(m_features.Features(currentShares > 0.0 ? 1.0 : 0.0, unrealized));

    if (action == 1 && currentShares == 0.0) return 1;
    if (action == 0 && currentShares > 0.0) return -1;
    return 0;
}
