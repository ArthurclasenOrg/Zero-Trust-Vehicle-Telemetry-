#include "queue.hpp"
#include <utility>

// thread 1 producing data
void Queue::produce(SqsMessageWrapper vehicleData)
{
    {
        unique_lock<mutex> lock(this->mtx);
        this->queue.push(std::move(vehicleData));
    }
    cv.notify_one(); // notifies thread 2 (consumer)
}

// thread 2 consuming data
bool Queue::consume(SqsMessageWrapper& vehicleDataOut)
{
    unique_lock<mutex> lock(this->mtx);

    // waiting for queue to not be empty or queue to be finished 
    this->cv.wait(lock, [this]() {return !this->queue.empty() || this->ended; });

    // there's no more itens or queue has ended
    if (this->queue.empty() && this->ended) return false;

    vehicleDataOut = move(queue.front());
    queue.pop();
    return true;
}

// finish queue
/* (this function will be used after...)
void Queue::finish()
{
    {
        unique_lock<mutex> lock(this->mtx);
        this->ended = true;
    }
    cv.notify_all(); // unlocks the consumer if waiting
}
*/ 