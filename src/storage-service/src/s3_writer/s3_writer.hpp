#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <iostream>
#include <memory>
#include <sstream>
#include "../../../common/include/vehicle_telemetry_state.hpp"

#ifndef S3_WRITER_HPP
#define S3_WRITER_HPP

class S3Writer
{
private:
    const Aws::String& fileKey;
    const VehicleTelemetryState vehicle;
    std::shared_ptr<Aws::S3::S3Client> s3Client;

    Aws::String generateFileContent() const;
    Aws::String getBucketName() const;
public:
    S3Writer(const Aws::String& fileKey, const VehicleTelemetryState& vehicle, 
        Aws::Client::ClientConfiguration clientConfig);
    
    bool s3Write() const;
};

#endif 