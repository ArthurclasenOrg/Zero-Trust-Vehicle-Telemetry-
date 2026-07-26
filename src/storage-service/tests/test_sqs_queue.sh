cd ../

# applying .env
export $(xargs < .env)

echo "searching on " $QUEUE_URL

aws sqs send-message \
  --queue-url "$QUEUE_URL" \
  --message-body '{"vehicle_id":"vehicle-01","status":"ok","speed_kph":80.5,"rpm":2500,"engine_temp_c":90.2,"diagnostic_code":"","schema_version":1,"last_seen_epoch_ms":1721490000000}' \
  --profile $PROFILE