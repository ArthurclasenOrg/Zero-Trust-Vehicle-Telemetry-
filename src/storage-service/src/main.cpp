
#include <aws/core/Aws.h>
#include <string>
#include "handler/handler.hpp"
#include <aws/sqs/model/GetQueueUrlRequest.h>
#include <aws/core/utils/logging/LogLevel.h>
#include <aws/s3/S3Client.h>

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

    // setting all the clients 
    auto sqsClient = std::make_shared<Aws::SQS::SQSClient>(clientConfig);
    auto dynamoClient = std::make_shared<Aws::DynamoDB::DynamoDBClient>(clientConfig);
    auto s3Client = std::make_shared<Aws::S3::S3Client>(clientConfig);

    // processing from sqs queue and deleting message later
    auto handler = std::make_unique<Handler>(queueUrl, sqsClient, dynamoClient, s3Client);
    thread t1_data_producer(&Handler::startPolling, handler.get());
    thread t2_data_consumer(&Handler::handleVehicleData, handler.get());

    // starting threads
    t1_data_producer.join();
    t2_data_consumer.join();  

    Aws::ShutdownAPI(options); // shutting down when ended
    return 0;
}