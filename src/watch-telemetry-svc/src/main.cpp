
#include <aws/core/Aws.h>
#include <string>
#include "handler/handler.hpp"
#include <aws/core/utils/logging/LogLevel.h>
#include <aws/dynamodb/DynamoDBClient.h>

using namespace aws::lambda_runtime;

int main()
{
    Aws::SDKOptions options; 
    // used to generate logs if needed
    // options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    Aws::InitAPI(options); // initializing API

    // creating my handler object
    auto handler = make_unique<Handler>();
    
    // running my lambda func (handleRequest)
    run_handler([&handler](invocation_request const& request){
        return handler->handleRequest(request);
    });

    Aws::ShutdownAPI(options); // shutting down when ended
    return 0;
}

