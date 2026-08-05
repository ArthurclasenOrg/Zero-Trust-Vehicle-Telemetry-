#include "handler.hpp"
#include <aws/dynamodb/model/GetItemRequest.h>
#include <aws/dynamodb/model/QueryRequest.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <iostream>

using namespace aws::lambda_runtime;

Handler::Handler(){}

invocation_response Handler::handleRequest(invocation_request const& request)
{
    // payload request has the JSON sent by the API Gateway
    Aws::Utils::Json::JsonValue json_payload(request.payload.c_str());
    Aws::Utils::Json::JsonView view = json_payload.View();

    // extracting which endpoint was accessed 
    Aws::String rawPath = view.KeyExists("rawPath") ? view.GetString("rawPath") : "";
    auto queryParams = view.KeyExists("queryStringParameters") ? view.GetObject("queryStringParameters") : Aws::Utils::Json::JsonView();

    if (!queryParams.IsNull() && queryParams.KeyExists("vehicle_id")) {   // routes to getVehicle
        // searching this vehicle
        return getVehicleState(queryParams.GetString("vehicle_id"));
    } else if (rawPath == "/anomalies") {  // routes to vehicles with anomalies (GSI)
        // returning anomlaies vehicles
        return getAnomalies();
    } else { // not found (404) 
        Aws::Utils::Json::JsonValue errorBody;
        errorBody.WithInteger("statusCode", 404);
        errorBody.WithString("body", "{\"error\": \"Path not found\"}");
        return invocation_response::success(errorBody.View().WriteCompact(), "application/json");
    }
}

// getting a specific vehicle state
invocation_response Handler::getVehicleState(const std::string& vehicleId)
{
    // setting the client (region = us-east-1)
    Aws::Client::ClientConfiguration clientConfig;
    clientConfig.region = "us-east-1";

    // declaring my vehicle object
    auto vehicle = make_unique<VehicleTelemetryState>();
    vehicle->vehicle_id = vehicleId;

    // creating dynamo object to search 
    const Aws::String tableName = "vehicle-telemetry-state";
    auto dynamo = make_unique<DynamoReader>(clientConfig);

    // searching for vehicle
    if (dynamo->dynamoReadSingleKey(*vehicle)){
        cout << "Vehicle found:" << endl;
        cout << vehicle->toString() << endl;

        // vehicle attributes to body
        Aws::Utils::Json::JsonValue vehicleJson;
        vehicleJson.WithString("vehicle_id", vehicle->vehicle_id);
        vehicleJson.WithString("status", vehicle->status);
        vehicleJson.WithDouble("speed_kph", vehicle->speed_kph);
        vehicleJson.WithInteger("rpm", vehicle->rpm);
        vehicleJson.WithDouble("engine_temp_c", vehicle->engine_temp_c);
        vehicleJson.WithInt64("last_seen_epoch_ms", vehicle->last_seen_epoch_ms);

        Aws::Utils::Json::JsonValue okBody;
        okBody.WithInteger("statusCode", 200);
        okBody.WithString("body", vehicleJson.View().WriteCompact());
        return invocation_response::success(okBody.View().WriteCompact(), "application/json");
    }  

    Aws::Utils::Json::JsonValue errorBody;
    errorBody.WithInteger("statusCode", 404);
    errorBody.WithString("body", "{\"error\": \"Vehicle not found.\"}");
    return invocation_response::success(errorBody.View().WriteCompact(), "application/json");

}

// getting all vehicles with anomalies
invocation_response Handler::getAnomalies()
{
    // setting the client (region = us-east-1)
    Aws::Client::ClientConfiguration clientConfig;
    clientConfig.region = "us-east-1";

    // declaring my vector
    auto vehicles = vector<VehicleTelemetryState>();

    // creating dynamo object to query
    const Aws::String tableName = "vehicle-telemetry-state";
    auto dynamo = make_unique<DynamoReader>(clientConfig);

    // searching for vehicles based on anomaly
    if (dynamo->dynamoReadDoubleKey(vehicles)){
        cout << "Vehicles found: " << endl;        
        Aws::Utils::Array<Aws::Utils::Json::JsonValue> vehicleArray(vehicles.size());
        size_t idx = 0;
        for (auto vehicle : vehicles){
            cout << vehicle.toString() << endl;
            // vehicles attribute
            Aws::Utils::Json::JsonValue vehicleJson;
            vehicleJson.WithString("vehicle_id", vehicle.vehicle_id);
            vehicleJson.WithString("status", vehicle.status);
            vehicleJson.WithDouble("speed_kph", vehicle.speed_kph);
            vehicleJson.WithInteger("rpm", vehicle.rpm);
            vehicleJson.WithDouble("engine_temp_c", vehicle.engine_temp_c);
            vehicleJson.WithInt64("last_seen_epoch_ms", vehicle.last_seen_epoch_ms);
            vehicleArray[idx++] = vehicleJson;
        }
        // I found all vehicles
        Aws::Utils::Json::JsonValue okBody;
        okBody.WithInteger("statusCode", 200);
        Aws::Utils::Json::JsonValue arrayWrapper;   // array of all JSONs
        arrayWrapper.AsArray(vehicleArray);
        okBody.WithString("body", arrayWrapper.View().WriteCompact());
        return invocation_response::success(okBody.View().WriteCompact(), "application/json");
    }

    Aws::Utils::Json::JsonValue errorBody;
    errorBody.WithInteger("statusCode", 404);
    errorBody.WithString("body", "{\"error\": \"Vehicles not found.\"}");
    return invocation_response::success(errorBody.View().WriteCompact(), "application/json");
    
}
