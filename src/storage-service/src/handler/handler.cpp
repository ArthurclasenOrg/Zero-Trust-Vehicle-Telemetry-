#include "handler.hpp"

using namespace std; 
using namespace aws::lambda_runtime;
using namespace Aws::Utils::Json;

StorageHandler::StorageHandler(){}

unique_ptr<VehicleTelemetryState> StorageHandler::parseJsonToPointer(const Aws::String& jsonBody)
{
    Aws::Utils::Json::JsonValue jsonValue(jsonBody);

    if (!jsonValue.WasParseSuccessful()){   // in case it wasn't correctly parsed
        cerr << "Error on realizing parse on JSON" << endl;
        return nullptr;
    }

    Aws::Utils::Json::JsonView view = jsonValue.View();

    // creating the object in the memory heap using smart pointer
    auto data = std::make_unique<VehicleTelemetryState>();

    // extracting JSON fields to the pointer to the data object
    if (view.ValueExists("vehicle_id"))         data->vehicle_id         = view.GetString("vehicle_id");       
    if (view.ValueExists("status"))             data->status             = view.GetString("status");
    if (view.ValueExists("speed_kph"))          data->speed_kph          = view.GetDouble("speed_kph");
    if (view.ValueExists("rpm"))                data->rpm                = view.GetInteger("rpm");
    if (view.ValueExists("engine_temp_c"))      data->engine_temp_c      = view.GetDouble("engine_temp_c");
    if (view.ValueExists("diagnostic_code"))    data->diagnostic_code    = view.GetString("diagnostic_code");
    if (view.ValueExists("schema_version"))     data->schema_version     = view.GetInteger("schema_version");
    if (view.ValueExists("last_seen_epoch_ms")) data->last_seen_epoch_ms = view.GetInt64("last_seen_epoch_ms");

    return data; // returning the smart pointer with the data correctly allocated
}

Aws::String buildS3Key(const VehicleTelemetryState& telemetry) {
    // Exemplo do formato: vehicles/vehicle-01/1721490000000.json
    return "vehicles/" + telemetry.vehicle_id + "/" + 
           std::to_string(telemetry.last_seen_epoch_ms) + ".json";
}

invocation_response StorageHandler::initializeHandler(invocation_request const& request)
{
    try {
        // setting client config
        Aws::Client::ClientConfiguration config;
        config.region = "us-east-1";

        // vehicle info
        unique_ptr<VehicleTelemetryState> vehicle;

        // parsing payload 
        JsonValue json_value(request.payload);
        JsonView json_view = json_value.View();

        // verifies if it has errors on json
        if (!json_value.WasParseSuccessful()){
            cerr << "Error parsing JSON." << endl;
            return invocation_response::failure(string("Error on JSON parsing"), string("ParseError"));
        }

        // verifies if event has records (it must come on events)
        if (!json_view.ValueExists("Records")) {
            cerr << "Invalid event or no messages (Records)." << endl;
        }

        Aws::Utils::Array<JsonView> records = json_view.GetArray("Records");

        // going through each record (message)
        for (size_t i = 0; i < records.GetLength(); i++) {
            JsonView record = records[i];

            // getting body (vehicle) 
            string body = record.ValueExists("body") ? record.GetString("body").c_str() : "";
        
            // parsing the json to a vehicle object
            vehicle = parseJsonToPointer(body);
            if (!vehicle) {
                cerr << "Fail on parsing vehicle JSON" << endl;
                continue;
            }
            
            // printing vehicle informations
            cout << vehicle->toString() << endl;

            // putting on the anomaly detector
            bool isAnomaly = detectAnomaly(*vehicle);
            if (isAnomaly) cout << "anomaly!" << endl;

            // writing on dynamoDB 
            auto dyanmo = std::make_unique<DynamoWritter>(*vehicle, config);
            if (dyanmo->dynamoWrite()) cout << "Written on dynamo." << endl;
            
            // writing on the s3 bucket
            const Aws::String fileKey = buildS3Key(*vehicle);
            auto s3bucket = std::make_unique<S3Writer>(fileKey, *vehicle, config);
            if (s3bucket->s3Write()) cout << "Written on bucket" << endl;
        }

        cout << "Finished processing. AWS is cleaning the queue" << endl;
        return invocation_response::success(string("Batch processed with success"), string("application/json"));

    } catch (const exception& e) {
        cerr << "Fatal error on handler: " << e.what() << endl;
        return invocation_response::failure(string(e.what()), string("InternalError"));
    }
}