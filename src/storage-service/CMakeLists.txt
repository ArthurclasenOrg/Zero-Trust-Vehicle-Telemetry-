cmake_minimum_required(VERSION 3.10)

# project name
project(StoreCarService CXX)

# C++17 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# pointing to the AWS SDK and AWS LAMBDA lib used
list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}/aws-sdk-install")

# searching for AWS packets
find_package(AWSSDK REQUIRED COMPONENTS sqs dynamodb s3)

# aws-core needs zlib
find_package(ZLIB REQUIRED)

# searching for threads native lib
find_package(Threads REQUIRED)

# defining running object and files (.cpp)
add_executable(storage_service
    src/main.cpp 
    src/handler/handler.cpp 
    src/handler/queue.cpp
    src/dynamo_writer/dynamo_writer.cpp 
    src/anomaly_detector/anomaly_detector.cpp
    src/s3_writer/s3_writer.cpp
)

# telling compiler where to search for headers (.hpp)
target_include_directories(storage_service PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
)

# linking AWS SDK and the thread lib to running
target_link_libraries(storage_service PRIVATE 
    ${AWSSDK_LINK_LIBRARIES} 
    Threads::Threads
)