
#include <aws/core/Aws.h>
#include <string>
#include "handler/handler.hpp"
#include <aws/core/utils/logging/LogLevel.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include "../../common/include/is_test.hpp"

using namespace aws::lambda_runtime;

int main()
{
    Aws::SDKOptions options; 
    // used to generate logs if needed
    // options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    Aws::InitAPI(options); // initializing API

    if (is_test().compare("yes") == 0){ // this is for github actions, this is for testing
        Handler handler{};

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
        return 0;
    }

    // creating my handler object
    auto handler = make_unique<Handler>();
    
    // running my lambda func (handleRequest)
    run_handler([&handler](invocation_request const& request){
        return handler->handleRequest(request);
    });

    Aws::ShutdownAPI(options); // shutting down when ended
    return 0;
}


