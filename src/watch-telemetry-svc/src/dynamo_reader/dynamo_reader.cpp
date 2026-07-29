#include "dynamo_reader.hpp"

#define ATTR_VAL Aws::DynamoDB::Model::AttributeValue

using namespace std;

DynamoReader::DynamoReader(const Aws::String& tableName, 
                            shared_ptr<Aws::DynamoDB::DynamoDBClient> dynamoClient)
    :  tableName(tableName),
       dynamoClient(dynamoClient) {}

bool DynamoReader::dynamoReadSingleKey(VehicleTelemetryState& vehicle)
{
    // setting a request GetItem
    Aws::DynamoDB::Model::GetItemRequest request;
    request.SetTableName(this->tableName);

    // setting the primary key (partition key + sort key)
    Aws::DynamoDB::Model::AttributeValue pk;
    pk.SetS(vehicle.vehicle_id);
    request.AddKey("vehicle_id", pk);

    // reading the database
    auto outcome = this->dynamoClient->GetItem(request);

    if (!outcome.IsSuccess()) {  // if the request fails
        cerr << "Error on reading DynamoDB: " << outcome.GetError().GetMessage() << endl;
        return false;
    }

    // getting the map attributes
    const auto& item = outcome.GetResult().GetItem();

    if (item.empty()) {  // if item not found
        cout << "Register not found for vehicle " << vehicle.vehicle_id << endl;
        return false;
    }
    try {
        // converting numbers (strings) to primitives c++
        if (item.find("vehicle_id") != item.end()) vehicle.vehicle_id = item.at("vehicle_id").GetS();
        if (item.find("status") != item.end()) vehicle.status = item.at("status").GetS();
        if (item.find("speed_kph") != item.end()) vehicle.speed_kph = stod(item.at("speed_kph").GetN().c_str());
        if (item.find("rpm") != item.end()) vehicle.rpm = stoi(item.at("rpm").GetN().c_str());
        if (item.find("engine_temp_c") != item.end()) vehicle.engine_temp_c = stod(item.at("engine_temp_c").GetN().c_str());
        if (item.find("diagnostic_code") != item.end()) vehicle.diagnostic_code = item.at("diagnostic_code").GetS();
        if (item.find("schema_version") != item.end()) vehicle.schema_version = stoi(item.at("schema_version").GetN().c_str());
        if (item.find("last_seen_epoch_ms") != item.end()) vehicle.last_seen_epoch_ms = stoll(item.at("last_seen_epoch_ms").GetN().c_str());
    } catch (const std::exception& e) {
        cerr << "malformed camp for " << vehicle.vehicle_id << ":" << e.what() << endl;
    }
    return true;
}

bool DynamoReader::dynamoReadDoubleKey(std::vector<VehicleTelemetryState>& vehicles)
{
    // setting a request Query
    Aws::DynamoDB::Model::QueryRequest request;
    request.SetTableName(this->tableName);

    // setting index name
    request.SetIndexName("anomaly-status-index");

    // condition to partition key
    request.SetKeyConditionExpression("#s = :statusVal");
    request.AddExpressionAttributeNames("#s", "status");

    // changing expression variables
    Aws::DynamoDB::Model::AttributeValue statusVal;
    statusVal.SetS("anomaly");
    request.AddExpressionAttributeValues(":statusVal", statusVal);

    // querying item
    auto outcome = this->dynamoClient->Query(request);

    if (outcome.IsSuccess()) {
        const auto& items = outcome.GetResult().GetItems();

        for (const auto& item : items) {
            VehicleTelemetryState vehicle;
            try {
                // converting numbers (strings) to primitives c++
                if (item.find("vehicle_id") != item.end()) vehicle.vehicle_id = item.at("vehicle_id").GetS();
                if (item.find("status") != item.end()) vehicle.status = item.at("status").GetS();
                if (item.find("speed_kph") != item.end()) vehicle.speed_kph = stod(item.at("speed_kph").GetN().c_str());
                if (item.find("rpm") != item.end()) vehicle.rpm = stoi(item.at("rpm").GetN().c_str());
                if (item.find("engine_temp_c") != item.end()) vehicle.engine_temp_c = stod(item.at("engine_temp_c").GetN().c_str());
                if (item.find("diagnostic_code") != item.end()) vehicle.diagnostic_code = item.at("diagnostic_code").GetS();
                if (item.find("schema_version") != item.end()) vehicle.schema_version = stoi(item.at("schema_version").GetN().c_str());
                if (item.find("last_seen_epoch_ms") != item.end()) vehicle.last_seen_epoch_ms = stoll(item.at("last_seen_epoch_ms").GetN().c_str());
            } catch (const std::exception& e) {
                cerr << "malformed camp for " << vehicle.vehicle_id << ":" << e.what() << endl;
            }
            // adding on list
            vehicles.push_back(vehicle);
        }
    } else {
        cerr << "Fail on Query: " << outcome.GetError().GetMessage() << endl;
        return false;
    }
    return true;
}
