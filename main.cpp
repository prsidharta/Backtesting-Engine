/**
 * @file main.cpp
 * @author Patrick Sidharta
 * @brief Beginning stage of backtesting engine. Currently handles mock initialization and average computation
 */

#include <iostream>
#include <vector>
#include <stdexcept>

/**
 * @brief Calculates the average of a given vector
 * @param prices A vector of type double for which the average of all values inside will be calculated
 * @return The average of the values in prices
 * @throws std::invalid_argument if vector given is empty
 */
double calculate_average(std::vector<double>& prices){
    double sum = 0.0;

    if (prices.size() == 0){
        throw std::invalid_argument("Empty Vector");
    }

    for (int i = 0; i < prices.size(); i++){
        sum += prices[i];
    }

    return sum / prices.size();
}

int main(){

    std::vector<double> prices;

    for (int i = 0; i < 5; i++){
        prices.push_back(100 + (i * 5));
    }

    double result = calculate_average(prices);

    std::cout << result << std::endl;
    return 0;
}