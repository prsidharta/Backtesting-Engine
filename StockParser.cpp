#include "StockParser.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

std::vector<double> InputPrices(std::string filename) {

    std::ifstream pFile(filename);
    if (!pFile.is_open()) {
        throw std::runtime_error("Failure to open file.");
    }

    std::string line;
    while (std::getline(pFile, line)) {
        std::cout << line << std::endl;
    }

    pFile.close();
}