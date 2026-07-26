#include <string>
#include <aws/core/Aws.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/PutItemRequest.h>

// vehicle telemetry state info for DynamoDB
#ifndef VEHICLE_TELEMETRY_STATE_HPP
#define VEHICLE_TELEMETRY_STATE_HPP

using namespace std;

struct VehicleTelemetryState
{
    std::string vehicle_id; // PK   
    std::string status;     // "ok" / "anomaly" - hash key for GSI
    double speed_kph;
    int rpm;
    double engine_temp_c;
    std::string diagnostic_code;
    int schema_version;
    int64_t last_seen_epoch_ms;

    // converting to DynamoDB attribute value
    // isolating SDK from AWS to the rest of logic
    Aws::DynamoDB::Model::PutItemRequest toPutItemRequest(const std::string& tableName) const;
    static VehicleTelemetryState fromItem(const Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue>& item);

    // toString() method
    std::string toString() const {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2); // Garante 2 casas decimais para os doubles
        ss << "VehicleTelemetryState {\n"
           << "  vehicle_id: \"" << vehicle_id << "\",\n"
           << "  status: \"" << status << "\",\n"
           << "  speed_kph: " << speed_kph << " km/h,\n"
           << "  rpm: " << rpm << " RPM,\n"
           << "  engine_temp_c: " << engine_temp_c << " °C,\n"
           << "  diagnostic_code: \"" << diagnostic_code << "\",\n"
           << "  schema_version: " << schema_version << ",\n"
           << "  last_seen_epoch_ms: " << last_seen_epoch_ms << "\n"
           << "}";
        return ss.str();
    }
};


#endif