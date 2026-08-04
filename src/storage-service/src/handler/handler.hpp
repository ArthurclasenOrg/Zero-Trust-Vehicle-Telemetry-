
#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/s3/S3Client.h>
#include <iostream>
#include <memory>
#include <optional>
#include <aws/lambda-runtime/runtime.h>
#include "../anomaly_detector/anomaly_detector.hpp"
#include "../dynamo_writer/dynamo_writter.hpp"
#include "../s3_writer/s3_writer.hpp"

using namespace aws::lambda_runtime;

#ifndef HANDLER_HPP
#define HANDLER_HPP

class StorageHandler
{
public:
    StorageHandler();

    // method to parse json into the vehicle telemetry pointer  
    std::unique_ptr<VehicleTelemetryState> parseJsonToPointer(const Aws::String& jsonBody);

    // method that will run on lambda
    invocation_response initializeHandler(invocation_request const& request);
};

#endif