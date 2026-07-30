#!/bin/bash
set -e

echo "Sending message to QUEUE_URL: $QUEUE_URL"

TAG="${IMAGE_TAG:-latest}"

# sending 3 messages to sqs
aws sqs send-message \
  --queue-url "$QUEUE_URL" \
  --message-body '{"vehicle_id":"vehicle-01","status":"ok","speed_kph":80.5,"rpm":2500,"engine_temp_c":90.2,"diagnostic_code":"","schema_version":1,"last_seen_epoch_ms":1721490000000}' \

aws sqs send-message \
  --queue-url "$QUEUE_URL" \
  --message-body '{"vehicle_id":"vehicle-02","status":"anomaly","speed_kph":67.5,"rpm":6700,"engine_temp_c":100.2,"diagnostic_code":"","schema_version":1,"last_seen_epoch_ms":1742910000000}' \

aws sqs send-message \
  --queue-url "$QUEUE_URL" \
  --message-body '{"vehicle_id":"vehicle-03","status":"anomaly","speed_kph":66.5,"rpm":6500,"engine_temp_c":150.2,"diagnostic_code":"","schema_version":1,"last_seen_epoch_ms":1755910000000}' \

echo "Running container for processing messages in background..."

CONTAINER_ID=$(docker run -d \
  -e QUEUE_URL="$QUEUE_URL" \
  -e BUCKET_NAME="$BUCKET_NAME" \
  -e AWS_REGION="${AWS_REGION:-us-east-1}" \
  -e AWS_ACCESS_KEY_ID="$AWS_ACCESS_KEY_ID" \
  -e AWS_SECRET_ACCESS_KEY="$AWS_SECRET_ACCESS_KEY" \
  -e AWS_SESSION_TOKEN="$AWS_SESSION_TOKEN" \
  ztv-storage-service:${TAG})

timeout 30s docker logs -f $CONTAINER_ID || true

echo "Time is up! Forcing container to stop..."

docker rm -f $CONTAINER_ID

echo "Test finished successfully."