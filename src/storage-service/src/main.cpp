
#include <aws/core/Aws.h>
#include <string>
#include "handler/handler.hpp"
#include <aws/core/utils/logging/LogLevel.h>
#include <aws/s3/S3Client.h>

// region
const std::string region = "us-east-1";

using namespace aws::lambda_runtime;

int main()
{
    Aws::SDKOptions options; 
    // used to generate logs if needed
    // options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    Aws::InitAPI(options); // initializing API

    if (is_test().compare("yes") == 0){ // for github actions test
        cout << "[TEST MODE] local test simulating SQS..." << std::endl;
        
        auto handler = std::make_unique<StorageHandler>();

        aws::lambda_runtime::invocation_request request;
        request.payload = R"({
            "Records": [
                {
                    "body": "{\"vehicle_id\":\"vehicle-01\", \"status\":\"anomaly\", \"rpm\":6000}"
                }
            ]
        })";

        auto response = handler->initializeHandler(request);
        std::cout << "1. Sucess? " << response.is_success() << std::endl;
        std::cout << "2. Body: " << response.get_payload() << std::endl;
        
        Aws::ShutdownAPI(options);
        return 0;
    }

    // processing from sqs queue and deleting message later
    auto handler = std::make_unique<StorageHandler>();
    
     // running my lambda func (initializeHandler)
    run_handler([&handler](invocation_request const& request){
        return handler->initializeHandler(request);
    });

    Aws::ShutdownAPI(options); // shutting down when ended
    return 0;
}