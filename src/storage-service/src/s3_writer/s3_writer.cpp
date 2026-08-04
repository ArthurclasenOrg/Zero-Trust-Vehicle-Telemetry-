#include "s3_writer.hpp"

using namespace std;

S3Writer::S3Writer(const Aws::String& fileKey, 
            const VehicleTelemetryState& vehicle, 
            Aws::Client::ClientConfiguration clientConfig)
    : fileKey(fileKey),
      vehicle(vehicle),
      s3Client(make_shared<Aws::S3::S3Client>(clientConfig)) {}

Aws::String S3Writer::generateFileContent() const
{
    // create the empty JSON object
    Aws::Utils::Json::JsonValue jsonNode;

    // adding keys and values
    jsonNode.WithString("vehicle_id", this->vehicle.vehicle_id);
    jsonNode.WithString("status", this->vehicle.status);
    jsonNode.WithDouble("speed_kph", this->vehicle.speed_kph);
    jsonNode.WithInteger("rpm", this->vehicle.rpm);
    jsonNode.WithDouble("engine_temp_c", this->vehicle.engine_temp_c);
    jsonNode.WithString("diagnostic_code", this->vehicle.diagnostic_code);
    jsonNode.WithInteger("schema_version", this->vehicle.schema_version);
    jsonNode.WithInt64("last_seen_epoch_ms", this->vehicle.last_seen_epoch_ms);

    // setting the file content
    return jsonNode.View().WriteCompact();
}

bool S3Writer::s3Write() const
{
    // creating the request
    Aws::S3::Model::PutObjectRequest request;

    request.SetBucket(getBucketName());
    request.SetKey(this->fileKey);

    // generating file content (JSON)
    const auto fileContent = this->generateFileContent();

    // converting a string to a Stream in memory
    std::shared_ptr<Aws::IOStream> dataStream = 
        Aws::MakeShared<Aws::StringStream>("AllocateS3Stream", fileContent);

    // setting stream as the body to request
    request.SetBody(dataStream);

    // defining the type of content S3 will recognize (JSON)
    request.SetContentType("application/json");

    // making the write request to AWS
    auto outcome = this->s3Client->PutObject(request);

    // validates result
    if (outcome.IsSuccess()) {
        cout << "File '" << fileKey << " saved with success on bucket '"
             << endl;
        return true;
    } else {
        cerr << "Fail on saving on S3: " << outcome.GetError().GetMessage() << endl;
    }

    return false;
}

// method to get bucket name
Aws::String S3Writer::getBucketName() const
{
    // getting bucket name
    const char* envBuckName = std::getenv("BUCKET_NAME");
    if (!envBuckName) {
        std::cerr << "BUCKET_NAME not defined" << std::endl;
        return "";
    }
    cout << envBuckName << endl;
    return envBuckName;
}