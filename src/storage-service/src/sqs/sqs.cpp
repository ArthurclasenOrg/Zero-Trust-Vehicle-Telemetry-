#include "sqs.hpp"
/*
using namespace std;

Sqs::Sqs(Aws::String queueUrl, Aws::Client::ClientConfiguration clientConfig)
    : queueUrl(queueUrl),
      sqsClient(make_shared<Aws::SQS::SQSClient>(clientConfig)){}


// processing queue (it returns the message object with receiptHandle and body data)
optional<const Aws::SQS::Model::Message> Sqs::processSQSQueue()
{
    // configuring the request to push to 1 message
    Aws::SQS::Model::ReceiveMessageRequest request;

    request.SetQueueUrl(this->queueUrl);
    request.SetMaxNumberOfMessages(1);
    request.SetWaitTimeSeconds(5); // polling of 5 seconds

    // call to SQS
    auto outcome = this->sqsClient->ReceiveMessage(request);
    
    if (outcome.IsSuccess()){ // if I receive messages
        const auto& messages = outcome.GetResult().GetMessages();

        if (!messages.empty()){ // if the message is not empty
            const auto& message = messages[0];
            // returning message (with body and receipt handle)
            return message;
        } else {
            cout << "No messages on queue" << endl;
        }
    } else {
        cerr << "Error on searching message: " << outcome.GetError().GetMessage() << endl;
    }
    return nullopt; // empty object 
}

void Sqs::deleteMessage(Aws::String receiptHandle)
{
    Aws::SQS::Model::DeleteMessageRequest deleteRequest;
    deleteRequest.SetQueueUrl(this->queueUrl);  // deleting message from this queue
    deleteRequest.SetReceiptHandle(receiptHandle);

    auto deleteOutcome = sqsClient->DeleteMessage(deleteRequest); // deleting message
    if (deleteOutcome.IsSuccess()){     // if success on deleting
        cout << "Message removed from queue with success!" << endl;
    }
}
*/

// READ SQS.HPP !!!