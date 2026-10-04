#include "AWSConnection.h"
#include <fstream>
#include <iostream>
#include <curl/curl.h>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

AWSConnection::AWSConnection() {}

bool AWSConnection::FetchData(const std::string &ticker) {
    std::cout << "\n Fetching from AWS S3 Bucket...\n";

    CURL* curl;
    CURLcode res;
    std::string csvData;
    std::string keyName = ticker + ".csv";
    
    std::string baseUrl = "https://amr9e3phr9.execute-api.us-east-1.amazonaws.com/prod/data/";
    std::string targetUrl = baseUrl + keyName;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, targetUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &csvData);
        
        std::cout << "Requesting " << keyName << "...\n";
        res = curl_easy_perform(curl);
        
        if(res == CURLE_OK) {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            if(response_code == 200) {
                std::cout << "Writing...\n";
                std::string filepath = "data/" + keyName;
                std::ofstream local_file(filepath, std::ios::binary);
                local_file << csvData;
                local_file.close();
                curl_easy_cleanup(curl);
                return true;
            } else {
                std::cerr << "HTTP Error Code: " << response_code << "\n";
            }
        } else {
            std::cerr << "cURL Error: " << curl_easy_strerror(res) << "\n";
        }
        curl_easy_cleanup(curl);
    }
    return false;
}
