variable "project_name" {
  type = string
  description = "Prefix used to tag/scope IAM policies"
}

# Dyanmo tables resources
variable "db_public_arn" {
  type = string
  description = "ARN of the public data (pseudonym) for visitors"
}

variable "db_state_arn" {
  type = string
  description = "ARN of the state of the exact data"
}

# S3 Bucket resource
variable "bucket_arn" {
  type        = string
  description = "ARN of the telemetry S3 bucket, used to scope store-car-svc and public-panel-svc policies"
}

# SQS resource
variable "queue_arn" {
  type = string
  description = "ARN of the sqs (not the dead letter one)"
}

# iot cloudwatch log group
variable "cloudwatch_log_group_iot_arn" {
  type = string
  description = "ARN of cloudwatch log group of IoT"
}


