#include "anomaly_detector.hpp"

bool detectAnomaly(VehicleTelemetryState vehicle){
    return (vehicle.status != "ok");
}