#include "S_BollingerRSI.h"
#include <cmath>
#include <algorithm>

void S_BollingerRSI::UpdateWindow(double dayPrice) {
    if (prevDayPrice == -1.0) {
        bWindowPrices.push(dayPrice);
        prevDayPrice = dayPrice;
        return;
    }

    bWindowPrices.push(dayPrice);

    double diff = dayPrice - prevDayPrice;
    rsiGains.push(std::max(0.0, diff));
    rsiLosses.push(std::max(0.0, -diff));

    if (bWindowPrices.size() > bPeriod) {
        bWindowPrices.pop();
    }
    if (rsiGains.size() > rsiPeriod) {
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