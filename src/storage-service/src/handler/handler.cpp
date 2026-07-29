#include "handler.hpp"
#include "queue.hpp"
#include "../dynamo_writer/dynamo_writter.hpp"
#include "../s3_writer/s3_writer.hpp"

using namespace std; 

StorageHandler::StorageHandler(const Aws::String& queueUrl, 
    std::shared_ptr<Aws::SQS::SQSClient> sqsClient,
    std::shared_ptr<Aws::DynamoDB::DynamoDBClient> dynamoClient,
    std::shared_ptr<Aws::S3::S3Client> s3Client)
    : queueUrl(queueUrl),
      sqsClient(sqsClient),
      dynamoClient(dynamoClient),
      s3Client(s3Client) {}

// processing queue (it returns the message object with receiptHandle and body data)
optional<const Aws::SQS::Model::Message> StorageHandler::processSQSQueue()
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

unique_ptr<VehicleTelemetryState> StorageHandler::parseJsonToPointer(const Aws::String& jsonBody)
{
    Aws::Utils::Json::JsonValue jsonValue(jsonBody);

    if (!jsonValue.WasParseSuccessful()){   // in case it wasn't correctly parsed
        cerr << "Error on realizing parse on JSON" << endl;
        return nullptr;
    }

    Aws::Utils::Json::JsonView view = jsonValue.View();

    // creating the object in the memory heap using smart pointer
    auto data = std::make_unique<VehicleTelemetryState>();

    // extracting JSON fields to the pointer to the data object
    if (view.ValueExists("vehicle_id"))         data->vehicle_id         = view.GetString("vehicle_id");       
    if (view.ValueExists("status"))             data->status             = view.GetString("status");
    if (view.ValueExists("speed_kph"))          data->speed_kph          = view.GetDouble("speed_kph");
    if (view.ValueExists("rpm"))                data->rpm                = view.GetInteger("rpm");
    if (view.ValueExists("engine_temp_c"))      data->engine_temp_c      = view.GetDouble("engine_temp_c");
    if (view.ValueExists("diagnostic_code"))    data->diagnostic_code    = view.GetString("diagnostic_code");
    if (view.ValueExists("schema_version"))     data->schema_version     = view.GetInteger("schema_version");
    if (view.ValueExists("last_seen_epoch_ms")) data->last_seen_epoch_ms = view.GetInt64("last_seen_epoch_ms");

    return data; // returning the smart pointer with the data correctly allocated
}

void StorageHandler::deleteMessage(Aws::String receiptHandle)
{
    Aws::SQS::Model::DeleteMessageRequest deleteRequest;
    deleteRequest.SetQueueUrl(this->queueUrl);  // deleting message from this queue
    deleteRequest.SetReceiptHandle(receiptHandle);

    auto deleteOutcome = sqsClient->DeleteMessage(deleteRequest); // deleting message
    if (deleteOutcome.IsSuccess()){     // if success on deleting
        cout << "Message removed from queue with success!" << endl;
    }
}

// polling for thread 1 (consumer from sqs and producer of vehicle data for thread 2)
void StorageHandler::startPolling()
{
    cout << "Initializing SQS Polling (Ctrl+C) to stop" << endl;

    while (true) {
        // get json from sqs
        auto messageOpt = this->processSQSQueue();
    
        // if it catches something, process
        if (messageOpt.has_value()) {
            const auto& message = messageOpt.value();
            unique_ptr<VehicleTelemetryState> vehicle = parseJsonToPointer(message.GetBody());

            if (vehicle) {
                // creating message to put on queue
                SqsMessageWrapper vehicleDataMessage;
                vehicleDataMessage.receiptHandle = message.GetReceiptHandle();
                vehicleDataMessage.vehicleData = move(vehicle);
                queue.produce(move(vehicleDataMessage));
            }
        }
    }
}

Aws::String buildS3Key(const VehicleTelemetryState& telemetry) {
    // Exemplo do formato: vehicles/vehicle-01/1721490000000.json
    return "vehicles/" + telemetry.vehicle_id + "/" + 
           std::to_string(telemetry.last_seen_epoch_ms) + ".json";
}

// thread 2 deals with the rest of the job (detect anomaly->write on dynamoDB and S3->delete data from sqs) 
void StorageHandler::handleVehicleData()
{
    cout << "Initializing Data Consumption" << endl;

    // getting bucket name
    const char* envBuckName = std::getenv("BUCKET_NAME");
    if (!envBuckName) {std::cerr << "BUCKET_NAME not defined" << std::endl;return;}

    while (true) {
        SqsMessageWrapper message;
        queue.consume(message); // consuming message from queue
        
        // prints vehicle data information and receipt handle
        cout << message.vehicleData->toString() << endl;
        cout << message.receiptHandle << endl;
        
        // putting on the anomaly detector
        bool isAnomaly = detectAnomaly(*message.vehicleData);
        if (isAnomaly) cout << "anomaly!" << endl;
        
        // writing on dynamoDB (passing the name of table too)
        const Aws::String tableName = "vehicle-telemetry-state";
        auto dyanmo = std::make_unique<DynamoWritter>(tableName, *message.vehicleData, this->dynamoClient);
        dyanmo->dynamoWrite();

        // writing on the s3 bucket
        const Aws::String bucketName = envBuckName;
        const Aws::String fileKey = buildS3Key(*message.vehicleData);
        auto s3bucket = std::make_unique<S3Writer>(bucketName, fileKey, *message.vehicleData, this->s3Client);
        s3bucket->s3Write();

        this->deleteMessage(message.receiptHandle);
        cout << "Deleted Message with success" << endl;
    }
}