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
    std::shared_ptr<Aws::DynamoDB::DynamoDBClient> dynamoClient;
    
    Aws::String getTableName();
public:
    explicit DynamoReader(Aws::Client::ClientConfiguration clientConfig);

    bool dynamoReadSingleKey(VehicleTelemetryState& vehicle);
    bool dynamoReadDoubleKey(std::vector<VehicleTelemetryState>& vehicles);
};


#endif