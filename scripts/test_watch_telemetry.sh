#!/bin/bash
echo "Runing container in a safe way..."

TAG="${IMAGE_TAG:-latest}"

docker run --rm \
  -e AWS_REGION="${AWS_REGION:-us-east-1}" \
  -e AWS_ACCESS_KEY_ID="$AWS_ACCESS_KEY_ID" \
  -e AWS_SECRET_ACCESS_KEY="$AWS_SECRET_ACCESS_KEY" \
  -e AWS_SESSION_TOKEN="$AWS_SESSION_TOKEN" \
  -e IS_TEST="yes" \
  652026215591.dkr.ecr.us-east-1.amazonaws.com/watch-telemetry-lambda:latest