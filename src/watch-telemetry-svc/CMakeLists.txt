cmake_minimum_required(VERSION 3.10)

# project name
project(WatchTelemetryService CXX)

# C++17 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# pointing to the AWS SDK and AWS LAMBDA lib used
list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}/aws-sdk-install" "$ENV{HOME}/aws-lambda-install")

# searching for AWS packets
find_package(AWSSDK REQUIRED COMPONENTS dynamodb)
find_package(aws-lambda-runtime REQUIRED)

# aws-core needs zlib
find_package(ZLIB REQUIRED)

# defining running object and files (.cpp)
add_executable(watch_telemetry_local_test
    tests/local_test_main.cpp
    src/handler/handler.cpp 
    src/dynamo_reader/dynamo_reader.cpp 
)

# telling compiler where to search for headers (.hpp)
target_include_directories(watch_telemetry_local_test PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src 
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
)

# linking AWS SDK and the thread lib to running
target_link_libraries(watch_telemetry_local_test PRIVATE 
    ${AWSSDK_LINK_LIBRARIES} 
    AWS::aws-lambda-runtime
)