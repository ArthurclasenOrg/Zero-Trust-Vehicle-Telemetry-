#pragma once
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/lambda-runtime/runtime.h>
#include <iostream>
#include <string>
#include "vehicle_telemetry_state.hpp"
#include "../dynamo_reader/dynamo_reader.hpp"

class Handler
{
private:
    // dynamo client
    std::shared_ptr<Aws::DynamoDB::DynamoDBClient> dynamoClient;

    // GET /vehicle/{vehicle_id} -> last state from a vehicle
    aws::lambda_runtime::invocation_response getVehicleState(const std::string& vehicleId);

    // GET /anomalies -> query on GSI anomaly-status-index
    aws::lambda_runtime::invocation_response getAnomalies();

public:
    explicit Handler(std::shared_ptr<Aws::DynamoDB::DynamoDBClient> dynamoClient);

    aws::lambda_runtime::invocation_response handleRequest(
        aws::lambda_runtime::invocation_request const& request);
};

