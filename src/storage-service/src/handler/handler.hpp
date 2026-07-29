
#include <aws/sqs/SQSClient.h>
#include <aws/sqs/model/ReceiveMessageRequest.h>
#include <aws/sqs/model/ReceiveMessageResult.h>
#include <aws/sqs/model/DeleteMessageRequest.h>
#include <aws/s3/S3Client.h>
#include <iostream>
#include <memory>
#include <optional>
#include "queue.hpp"
#include "../anomaly_detector/anomaly_detector.hpp"

#ifndef HANDLER_HPP
#define HANDLER_HPP

class StorageHandler
{
private:
    const Aws::String queueUrl;
    std::shared_ptr<Aws::SQS::SQSClient> sqsClient;
    std::shared_ptr<Aws::DynamoDB::DynamoDBClient> dynamoClient;
    std::shared_ptr<Aws::S3::S3Client> s3Client;
    Queue queue;
public:
    StorageHandler(const Aws::String& queueUrl, 
        std::shared_ptr<Aws::SQS::SQSClient> sqsClient,
        std::shared_ptr<Aws::DynamoDB::DynamoDBClient> dynamoClient,
        std::shared_ptr<Aws::S3::S3Client> s3Client);

    // method to get data from queue
    std::optional<const Aws::SQS::Model::Message> processSQSQueue();
    // method to parse json into the vehicle telemetry pointer  
    std::unique_ptr<VehicleTelemetryState> parseJsonToPointer(const Aws::String& jsonBody);
    // deleting the data obtained from queue (the one we have already processed)
    void deleteMessage(Aws::String receiptHandle);
    
    // loop for thread 1 produce vehicle data
    void startPolling();

    // loop for thread 2 handle with the vehicle data from thread 1
    void handleVehicleData();
};

#endif