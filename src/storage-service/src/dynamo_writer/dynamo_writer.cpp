#include "dynamo_writter.hpp"

#define ATTR_VAL Aws::DynamoDB::Model::AttributeValue

using namespace std;

DynamoWritter::DynamoWritter(const Aws::String& tableName, 
                            VehicleTelemetryState& vehicle, 
                            std::shared_ptr<Aws::DynamoDB::DynamoDBClient> dynamoClient)
    :  tableName(tableName),
       vehicle(vehicle),
       dynamoClient(dynamoClient) {}

bool DynamoWritter::dynamoWrite()
{
    // preparing request object
    Aws::DynamoDB::Model::PutItemRequest putItemRequest;
    putItemRequest.SetTableName(this->tableName);

    // making the item (column by column)
    //--------------------------------------------
    // ID column (vehicle_id from dynamo.tf)
    ATTR_VAL idAttr;
    idAttr.SetS(this->vehicle.vehicle_id);
    putItemRequest.AddItem("vehicle_id", idAttr);

    // status column
    ATTR_VAL statusAttr;
    statusAttr.SetS(this->vehicle.status);
    putItemRequest.AddItem("status", statusAttr);

    // speed_kph column
    ATTR_VAL speedAttr;
    speedAttr.SetN(std::to_string(this->vehicle.speed_kph).c_str());
    putItemRequest.AddItem("speed_kph", speedAttr);

    // rpm column
    ATTR_VAL rpmAttr;
    rpmAttr.SetN(this->vehicle.rpm);
    putItemRequest.AddItem("rpm", rpmAttr);

    // engine temperature column
    ATTR_VAL engineTempCAttr;
    engineTempCAttr.SetN(std::to_string(this->vehicle.engine_temp_c).c_str());
    putItemRequest.AddItem("engine_temp_c", engineTempCAttr);

    // status diagnostic code column
    ATTR_VAL diagnosticCodeAttr;
    diagnosticCodeAttr.SetS(this->vehicle.diagnostic_code);
    putItemRequest.AddItem("diagnostic_code", diagnosticCodeAttr);
    
    // schema version column
    ATTR_VAL schemaVersionAttr;
    schemaVersionAttr.SetN(this->vehicle.schema_version);
    putItemRequest.AddItem("schema_version", schemaVersionAttr);

    // last seen epoch (ms) column
    ATTR_VAL lastSeenEpochMsAttr;
    lastSeenEpochMsAttr.SetN(std::to_string(this->vehicle.last_seen_epoch_ms).c_str());
    putItemRequest.AddItem("last_seen_epoch_ms", lastSeenEpochMsAttr);
    //--------------------------------------------

    // making the request (write) to AWS
    const auto outcome = this->dynamoClient->PutItem(putItemRequest);

    // validates if it had success
    if (outcome.IsSuccess()) {
        std::cout << "Register saved on DynamoDB." << std::endl;
        return true;
    } else {
        std::cerr << "Fail on saving on database: " << outcome.GetError().GetMessage() << std::endl;
    }
    return false;
}


