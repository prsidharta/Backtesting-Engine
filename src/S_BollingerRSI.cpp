#include "S_BollingerRSI.h"
#include <algorithm>
#include <cmath>
#include <numeric>

void S_BollingerRSI::UpdateWindow(double dayPrice) {
    if (prevDayPrice == -1.0) {
        bWindowPrices.push(dayPrice);
        bWindowSum = dayPrice;
        prevDayPrice = dayPrice;
        bWindowSumSquares = (dayPrice * dayPrice);
        return;
    }

    bWindowPrices.push(dayPrice);
    bWindowSum += dayPrice;

    double diff = dayPrice - prevDayPrice;
    double gain = (std::max(0.0, diff));
    double loss = (std::max(0.0, -diff));

    rsiGains.push(gain);
    rsiLosses.push(loss);

    rsiGainsSum += gain;
    rsiLossesSum += loss;

    bWindowSumSquares += (dayPrice * dayPrice);

    if (bWindowPrices.size() > bPeriod) {
        bWindowSum -= bWindowPrices.front();
        bWindowSumSquares -= (bWindowPrices.front() * bWindowPrices.front());
        bWindowPrices.pop();
    }
    if (rsiGains.size() > rsiPeriod) {
        rsiGainsSum -= rsiGains.front();
        rsiLossesSum -= rsiLosses.front();
        rsiGains.pop();
        rsiLosses.pop();
    }
    prevDayPrice = dayPrice;
    return;
}

S_BollingerRSI::S_BollingerRSI(int bPeriod, double bMultiplier, int rsiPeriod, int rsiUpper, int rsiLower) {
    this->bPeriod = bPeriod;
    this->bMultiplier = bMultiplier;
    this->rsiPeriod = rsiPeriod;
    this->rsiUpper = rsiUpper;
    this->rsiLower = rsiLower;
    prevDayPrice = -1.0;
}

double S_BollingerRSI::bMovingAverage() {
    if (bWindowPrices.empty()) {
        return 0.0;
    }

    return (bWindowSum / bPeriod);
}

double S_BollingerRSI::bStdDev() {
    if (bWindowPrices.empty()) {
        return 0.0;
    }

    double mean = bMovingAverage();

    double variance = (bWindowSumSquares / bPeriod) - (mean * mean);

    if (variance <= 0.0) {
        return 0.0;
    }

    return std::sqrt(variance);
}

double S_BollingerRSI::rsiFinal() {
    double avgGain = rsiGainsSum / rsiPeriod;
    double avgLoss = rsiLossesSum / rsiPeriod;

    if (avgLoss == 0.0) {
        return 100.0;
    }

    double rsi = avgGain / avgLoss;
    return 100 - (100 / (1.0 + rsi));
}

int S_BollingerRSI::CreateSignal(double dayPrice, double currentShares) {
    UpdateWindow(dayPrice);
    if (bWindowPrices.size() < bPeriod) {
        return 0;
    }

    double mean = bMovingAverage();
    double stdDev = bStdDev();
    double rsi = rsiFinal();

    double upperBand = mean + (bMultiplier * stdDev);
    double lowerBand = mean - (bMultiplier * stdDev);

    if (dayPrice < lowerBand && rsi < rsiLower){
        if (currentShares == 0){
            return 1;
        }
    }

    if (dayPrice > upperBand && rsi > rsiUpper){
        if (currentShares > 0){
            return -1;
        }
    }
    
    return 0;
}