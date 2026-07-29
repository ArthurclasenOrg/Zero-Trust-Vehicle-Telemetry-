#include <aws/core/Aws.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/GetItemRequest.h>
#include <aws/dynamodb/model/AttributeValue.h>
#include <iostream>
#include <memory>
#include "../../../common/include/vehicle_telemetry_state.hpp"

#ifndef DYNAMO_READER_HPP
#define DYNAMO_READER_HPP

class DynamoReader
{
private:
    const Aws::String& tableName;
    std::shared_ptr<Aws::DynamoDB::DynamoDBClient> dynamoClient;
public:
    DynamoReader(const Aws::String& tableName, 
        std::shared_ptr<Aws::DynamoDB::DynamoDBClient> dynamoClient);

    bool dynamoReadSingleKey(VehicleTelemetryState& vehicle);
    bool dynamoReadDoubleKey(std::vector<VehicleTelemetryState>& vehicles);
};


#endif