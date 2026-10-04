#include "FeatureBuilder.h"
#include <algorithm>
#include <cmath>

namespace {
constexpr int kSmaPeriod = 20;
constexpr int kRsiPeriod = 14;
constexpr double kBandMultiplier = 2.0;

double Clip(double x) {
    return std::max(-5.0, std::min(5.0, x));
}
} // namespace

void FeatureBuilder::Reset() {
    m_prices.clear();
}

void FeatureBuilder::Update(double price) {
    m_prices.push_back(price);
    if (m_prices.size() > kWarmup + 1) {
        m_prices.pop_front();
    }
}

bool FeatureBuilder::Ready() const {
    return m_prices.size() >= kWarmup + 1;
}

std::vector<double> FeatureBuilder::Features(double position, double unrealizedPct) const {
    std::vector<double> f(kNumFeatures, 0.0);
    if (!Ready()) {
        return f;
    }

    const size_t n = m_prices.size();
    const double p = m_prices[n - 1];

    // Log returns over 1 / 5 / 20 bars, divided by a typical magnitude to get ~unit scale
    f[0] = Clip(std::log(p / m_prices[n - 2]) / 0.01);
    f[1] = Clip(std::log(p / m_prices[n - 6]) / 0.025);
    f[2] = Clip(std::log(p / m_prices[n - 21]) / 0.05);

    // Price relative to its 20-bar SMA, and Bollinger %B (the 20 bars ending at the current one)
    double sum = 0.0;
    for (size_t i = n - kSmaPeriod; i < n; i++) {
        sum += m_prices[i];
    }
    const double mean = sum / kSmaPeriod;
    double sq = 0.0;
    for (size_t i = n - kSmaPeriod; i < n; i++) {
        sq += (m_prices[i] - mean) * (m_prices[i] - mean);
    }
    const double stdDev = std::sqrt(sq / kSmaPeriod);

    f[3] = Clip((p / mean - 1.0) / 0.03);
    if (stdDev > 0.0) {
        const double lower = mean - kBandMultiplier * stdDev;
        const double width = 2.0 * kBandMultiplier * stdDev;
        f[4] = Clip(((p - lower) / width - 0.5) * 2.0); // %B rescaled so [0,1] -> [-1,1]
    }

    // RSI over the last 14 price changes
    double gains = 0.0;
    double losses = 0.0;
    for (size_t i = n - kRsiPeriod; i < n; i++) {
        const double diff = m_prices[i] - m_prices[i - 1];
        gains += std::max(0.0, diff);
        losses += std::max(0.0, -diff);
    }
    double rsi = 100.0;
    if (losses > 0.0) {
        rsi = 100.0 - 100.0 / (1.0 + gains / losses);
    }
    f[5] = (rsi - 50.0) / 25.0;

    f[6] = position;
    f[7] = Clip(unrealizedPct / 0.05);
    return f;
}
