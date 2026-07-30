#!/bin/bash
echo "Runing container in a safe way..."

docker run --rm \
  -e AWS_REGION="${AWS_REGION:-us-east-1}" \
  -e AWS_ACCESS_KEY_ID="$AWS_ACCESS_KEY_ID" \
  -e AWS_SECRET_ACCESS_KEY="$AWS_SECRET_ACCESS_KEY" \
  -e AWS_SESSION_TOKEN="$AWS_SESSION_TOKEN" \
  ztv-watch-telemetry-svc