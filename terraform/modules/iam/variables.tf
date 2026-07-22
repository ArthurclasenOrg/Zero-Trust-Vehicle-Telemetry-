variable "project_name" {
  type = string
  description = "Prefix used to tag/scope IAM policies"
}

variable "bucket_arn" {
  type        = string
  description = "ARN of the telemetry S3 bucket, used to scope store-car-svc and public-panel-svc policies"
}