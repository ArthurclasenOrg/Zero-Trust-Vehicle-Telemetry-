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
    Aws::Client::ClientConfiguration clientConfig;
public:
    DynamoWritter(const Aws::String& tableName, VehicleTelemetryState& vehicle, Aws::Client::ClientConfiguration& clientConfig);

    bool dynamoWrite();
};


#endif