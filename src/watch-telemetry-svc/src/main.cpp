
#include <aws/core/Aws.h>
#include <string>
#include "handler/handler.hpp"
#include <aws/core/utils/logging/LogLevel.h>
#include <aws/dynamodb/DynamoDBClient.h>

// region
const std::string region = "us-east-1";
using namespace aws::lambda_runtime;

int main()
{
    Aws::SDKOptions options; 
    // used to generate logs if needed
    // options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    Aws::InitAPI(options); // initializing API

    // setting the client (region = us-east-1)
    Aws::Client::ClientConfiguration clientConfig;
    clientConfig.region = region;

    // setting all the clients 
    auto dynamoClient = std::make_shared<Aws::DynamoDB::DynamoDBClient>(clientConfig);

    // creating my handler object
    auto handler = make_unique<Handler>(dynamoClient);
    
    // running my lambda func (handleRequest)
    run_handler([&handler](invocation_request const& request){
        return handler->handleRequest(request);
    });

    Aws::ShutdownAPI(options); // shutting down when ended
    return 0;
}