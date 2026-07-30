#!/bin/bash
echo "Runing container in a safe way..."

docker run --rm \
  --env-file ../.env.aws \
  ztv-watch-telemetry-svc