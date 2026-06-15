#pragma once

#include <string>

class AWSConnection {
    public:
        AWSConnection();
        bool FetchData(const std::string& ticker);
};