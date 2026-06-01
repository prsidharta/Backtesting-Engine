#include "StockParser.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cctype>

std::vector<double> ReadCsv(std::string filename) {

    std::ifstream csvFile(filename);
    std::string line, textChunk;
    std::vector<double> stockPrices;
    if (!csvFile.is_open()) {
        throw std::runtime_error("Failure to open file.");
    }

    while (std::getline(csvFile, line)) {

        if (line.empty() || !std::isdigit(line[0])){
            continue;
        }
        int i = 0;
        std::stringstream lineStream(line);

        while (std::getline(lineStream, textChunk, ',')) {
            if (i == 4) {
                stockPrices.push_back(std::stod(textChunk));
                break;
            }
            i++;
        }
    }
    csvFile.close();
    return stockPrices;
}