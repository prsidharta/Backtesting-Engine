#include "AWSConnection.h"
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <fstream>
#include <iostream>

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