#pragma once

#include <string>
#include <vector>

/**
 * @brief Parses a Yahoo finance CSV file to extract stock closing prices
 * @param filename The local name of the CSV file
 * @return std::vector<double> A chronological list of closing prices extracted from the given CSV file
 * @throws std::runtime_error If the file given to the function fails to open
 */
std::vector<double> InputPrices(std::string filename);