#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "../../../common/include/vehicle_telemetry_state.hpp"

#ifndef QUEUE_HPP
#define QUEUE_HPP

using namespace std;

struct SqsMessageWrapper {
    std::unique_ptr<VehicleTelemetryState> vehicleData;
    Aws::String receiptHandle;

    //constructor 
    SqsMessageWrapper() = default;

    // allowing it to "move"
    SqsMessageWrapper(SqsMessageWrapper&&) noexcept = default;
    SqsMessageWrapper& operator=(SqsMessageWrapper&&) noexcept = default;

    // deleting the copies so compiler doesn't try to do it
    SqsMessageWrapper(const SqsMessageWrapper&) = delete;
    SqsMessageWrapper& operator=(const SqsMessageWrapper&) = delete;
};

class Queue 
{
private:
    std::queue<SqsMessageWrapper> queue;
    std::mutex mtx;
    std::condition_variable cv;
    bool ended = false;
public:
    void produce(SqsMessageWrapper vehicleData);
    bool consume(SqsMessageWrapper& vehicleDataOut);
    void finish();
};

#endif