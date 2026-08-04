#include <aws/sqs/SQSClient.h>
#include <aws/sqs/model/ReceiveMessageRequest.h>
#include <aws/sqs/model/ReceiveMessageResult.h>
#include <aws/sqs/model/DeleteMessageRequest.h>
#include <aws/core/Aws.h>
#include <optional>

#ifndef SQS_HPP
#define SQS_HPP

using namespace std;

// 
//  DISCLAIMER !!!
//  this class is not used by any method because i am using a 
//  lambda application with an event triggering and it calls the
//  queue automatically for me, but if I were going to use an EC2
//  this class would be necessary.
// 
class Sqs
{
private:
    const Aws::String queueUrl;
    std::shared_ptr<Aws::SQS::SQSClient> sqsClient;
public:
    Sqs(Aws::String queueUrl, Aws::Client::ClientConfiguration clientConfig);

    // method to get data from queue
    optional<const Aws::SQS::Model::Message> processSQSQueue();

    // deleting the data obtained from queue (the one we have already processed)
    void deleteMessage(Aws::String receiptHandle);
};

#endif