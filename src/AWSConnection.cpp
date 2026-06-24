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

/*
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>
AWSConnection::AWSConnection() {}

bool AWSConnection::FetchData(const std::string &ticker) {
    std::cout << "\n Authenticating...\n";

    Aws::Client::ClientConfiguration clientConfig;
    clientConfig.region = "us-east-1";

    Aws::S3::S3Client s3_client(clientConfig);

    Aws::S3::Model::GetObjectRequest object_request;
    object_request.SetBucket("psidharta-backtest-data-2026");

    std::string keyName = ticker + ".csv";
    object_request.SetKey(keyName);

    std::cout << "Requesting " << keyName << "...\n";

    auto get_object_outcome = s3_client.GetObject(object_request);

    if (get_object_outcome.IsSuccess()) {
        std::cout << "Writing...\n";

        std::string filepath = "data/" + keyName;
        std::ofstream local_file(filepath, std::ios::binary);
        local_file << get_object_outcome.GetResult().GetBody().rdbuf();
        local_file.close();

        return true;
    } else {
        std::cerr << "AWS S3 Error: " << get_object_outcome.GetError().GetMessage() << "\n";
        return false;
    }
}
*/