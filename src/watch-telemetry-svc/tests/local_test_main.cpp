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
            "queryStringParameters": { "vehicle_id": "vehicle-01" }
        })";

        auto response = handler.handleRequest(request);
        std::cout << "Sucess? " << response.is_success() << std::endl;
        std::cout << "Body: " << response.get_payload() << std::endl;
    }
    Aws::ShutdownAPI(options);
    return 0;
}