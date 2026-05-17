/**
 * @file main.cpp
 * @author Patrick Sidharta
 * @brief Beginning stage of backtesting engine. Currently handles mock initialization and average computation
 */

#include <iostream>
#include <vector>
#include <stdexcept>


/**
 * @brief Calculates a window_size day average trend for each day
 * @param prices, A vector of listed prices of stock for all days 
 * @param window_size, The number of days a trend average is calculated for
 * @throw std::invalid_argument if prices vector is empty
 * @throw std::invalid_argument if window_size is greater than total number of prices
 * @return std::vector<double> containing the calculated window averages
 */
std::vector<double> window_average_calculator(std::vector<double>& prices, int window_size){

    if (prices.size() == 0){
        throw std::invalid_argument("Empty Vector");
    }

    if (window_size > prices.size()){
        throw std::invalid_argument("Requested Window Period is Greater than Vector Provided");
    }

    std::vector<double> window_averages;

    for (int i = 0; i < prices.size() - (window_size - 1); i++){
        double temp = 0;
        for (int j = 0; j < window_size; j++){
            temp += prices[i + j];
        }

        temp = temp / window_size;
        window_averages.push_back(temp);
    }

    return window_averages;
}

/**
 * @brief Simulates a run of the backtesting engine
 * @param prices A vector of listed prices of stock for all days
 * @param window_averages A vector of daily average trends based on the window_size
 * @param[in,out] cash Simulated bank account of the user
 * @param[in,out] shares Number of simulated shares owned by the user
 * @param window_size The number of days a trend average is calculated for, used to align prices day and window_average day
 * @note shares set to double supports fractional share ownership, int shares does not (which is not applied here)
 */
void run_simulator(std::vector<double>& prices, std::vector<double>& window_averages,
                    double& cash, double& shares , int window_size){

    for (int day = 0; day < window_averages.size(); day++){
        int day_offset = day + (window_size - 1);

        if (shares == 0 && prices[day_offset] > window_averages[day]){
            shares = cash / prices[day_offset];
            cash = cash - (shares * prices[day_offset]);    
        }

        if(shares != 0 && prices[day_offset] < window_averages[day]){
            cash = cash + (shares * prices[day_offset]);
            shares = 0;
        }
    }
}

int main(){

    std::vector<double> prices;

    for (int i = 0; i < 5; i++){
        prices.push_back(100 + (i * 5));
    }

    return 0;
}