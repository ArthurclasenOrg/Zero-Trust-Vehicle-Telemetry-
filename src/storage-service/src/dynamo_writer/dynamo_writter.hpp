#include <aws/core/Aws.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/PutItemRequest.h>
#include <aws/dynamodb/model/PutItemResult.h>
#include <aws/dynamodb/model/AttributeValue.h>
#include <iostream>
#include <string>
#include "../../../common/include/vehicle_telemetry_state.hpp"

#ifndef DYNAMO_WRITTER_HPP
#define DYNAMO_WRITTER_HPP

class DynamoWritter
{
private:
    const Aws::String& tableName;
    VehicleTelemetryState vehicle;
    std::shared_ptr<Aws::DynamoDB::DynamoDBClient> dynamoClient;
public:
    DynamoWritter(const Aws::String& tableName, VehicleTelemetryState& vehicle, 
        std::shared_ptr<Aws::DynamoDB::DynamoDBClient> dynamoClient);

    bool dynamoWrite();
};


#endif