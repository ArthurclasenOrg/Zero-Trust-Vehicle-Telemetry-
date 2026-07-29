cd ../

# applying .env
export $(xargs < .env)

echo "searching on " $QUEUE_URL

aws sqs send-message \
  --queue-url "$QUEUE_URL" \
  --message-body '{"vehicle_id":"vehicle-01","status":"ok","speed_kph":80.5,"rpm":2500,"engine_temp_c":90.2,"diagnostic_code":"","schema_version":1,"last_seen_epoch_ms":1721490000000}' \
  --profile $PROFILE

aws sqs send-message \
  --queue-url "$QUEUE_URL" \
  --message-body '{"vehicle_id":"vehicle-02","status":"anomaly","speed_kph":67.5,"rpm":6700,"engine_temp_c":100.2,"diagnostic_code":"","schema_version":1,"last_seen_epoch_ms":1742910000000}' \
  --profile $PROFILE

aws sqs send-message \
  --queue-url "$QUEUE_URL" \
  --message-body '{"vehicle_id":"vehicle-03","status":"anomaly","speed_kph":66.5,"rpm":6500,"engine_temp_c":150.2,"diagnostic_code":"","schema_version":1,"last_seen_epoch_ms":1755910000000}' \
  --profile $PROFILE