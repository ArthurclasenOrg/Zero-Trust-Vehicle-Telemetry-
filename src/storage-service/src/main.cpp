
#include <aws/core/Aws.h>
#include <string>
#include "handler/handler.hpp"
#include <aws/sqs/model/GetQueueUrlRequest.h>
#include <aws/core/utils/logging/LogLevel.h>

// region
const std::string region = "us-east-1";

int main()
{
    Aws::SDKOptions options; 
    // used to generate logs if needed
    // options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    Aws::InitAPI(options); // initializing API

    // getting the queue url
    const char* envUrl = std::getenv("QUEUE_URL");
    if (!envUrl) {
        std::cerr << "QUEUE_URL not defined" << std::endl;
        return 1;
    }
    const Aws::String queueUrl = envUrl;

    // setting the client (region = us-east-1)
    Aws::Client::ClientConfiguration clientConfig;
    clientConfig.region = region;
    // clientConfig.verifySSL = false;

    // processing from sqs queue and deleting message later
    auto sqs_handler = std::make_unique<Handler>(queueUrl, clientConfig);
    thread t1_data_producer(&Handler::startPolling, sqs_handler.get());
    thread t2_data_consumer(&Handler::handleVehicleData, sqs_handler.get());

    // starting threads
    t1_data_producer.join();
    t2_data_consumer.join();

    Aws::ShutdownAPI(options); // shutting down when ended
    return 0;
}