
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

    // processing from sqs queue and deleting message later
    auto handler = std::make_unique<StorageHandler>();
    
     // running my lambda func (initializeHandler)
    run_handler([&handler](invocation_request const& request){
        return handler->initializeHandler(request);
    });

    Aws::ShutdownAPI(options); // shutting down when ended
    return 0;
}