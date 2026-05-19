#include "SimEngine.h"

#include <iostream>
#include <stdexcept>

std::vector<double> CalculateWindowAverages(std::vector<double> &prices, int windowSize) {

    if (prices.size() == 0) {
        throw std::invalid_argument("Empty Vector");
    }

    if (windowSize > prices.size()) {
        throw std::invalid_argument("Requested Window Period is Greater than Vector Provided");
    }

    std::vector<double> windowAverages;

    for (int i = 0; i < prices.size() - (windowSize - 1); i++) {
        double temp = 0;
        for (int j = 0; j < windowSize; j++) {
            temp += prices[i + j];
        }

        temp = temp / windowSize;
        windowAverages.push_back(temp);
    }

    return windowAverages;
}

void RunSimulator(std::vector<double> &prices, std::vector<double> &windowAverages, double &cash, double &shares,
                  int windowSize) {

    for (int day = 0; day < windowAverages.size(); day++) {
        int dayOffset = day + (windowSize - 1);

        if (shares == 0 && prices[dayOffset] > windowAverages[day]) {
            std::cout << "BUY+" << std::endl;
            std::cout << "Stock Price: " << prices[dayOffset] << std::endl;
            std::cout << "Moving Average: " << windowAverages[day] << std::endl;
            shares = cash / prices[dayOffset];
            cash = cash - (shares * prices[dayOffset]);
            std::cout << "Shares bought: " << shares << std::endl;
            std::cout << "Current Cash: " << cash << std::endl;
        }

        if (shares != 0 && prices[dayOffset] < windowAverages[day]) {
            std::cout << "SELL-" << std::endl;
            std::cout << "Stock Price: " << prices[dayOffset] << std::endl;
            std::cout << "Moving Average: " << windowAverages[day] << std::endl;
            std::cout << "Shares Sold: " << shares << std::endl;
            cash = cash + (shares * prices[dayOffset]);
            shares = 0;
            std::cout << "Current Cash: " << cash << std::endl;
        }
    }
}