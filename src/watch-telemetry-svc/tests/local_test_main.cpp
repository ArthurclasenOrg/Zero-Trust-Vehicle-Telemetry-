#include <iostream>
#include <aws/core/Aws.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include "../src/handler/handler.hpp"

int main() {
    Aws::SDKOptions options;
    Aws::InitAPI(options);
    {
        Aws::Client::ClientConfiguration clientConfig;
        clientConfig.region = "us-east-1";
        auto dynamoClient = std::make_shared<Aws::DynamoDB::DynamoDBClient>(clientConfig);
        Handler handler(dynamoClient);

        // simulating an event calling /?vehicle_id=vehicle-01
        aws::lambda_runtime::invocation_request request;
        request.payload = R"({
            "rawPath": "/",
            "queryStringParameters": { "vehicle_id": "vehicle-02" }
        })";

        auto response = handler.handleRequest(request);
        std::cout << "1. Sucess? " << response.is_success() << std::endl;
        std::cout << "2. Body: " << response.get_payload() << std::endl;
    
        // simulating an event calling /anomalies
        aws::lambda_runtime::invocation_request request2;
        request2.payload = R"({
            "rawPath": "/anomalies"
        })";
        auto response2 = handler.handleRequest(request2);
        std::cout << "1. Sucess? " << response2.is_success() << std::endl;
        std::cout << "2. Body: " << response2.get_payload() << std::endl;

    }
    Aws::ShutdownAPI(options);
    return 0;
}