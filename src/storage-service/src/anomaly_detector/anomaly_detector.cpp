#include "anomaly_detector.hpp"

bool detectAnomaly(const VehicleTelemetryState& vehicle){
    return (vehicle.status != "ok");
}