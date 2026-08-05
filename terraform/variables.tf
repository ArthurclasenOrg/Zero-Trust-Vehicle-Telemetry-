variable "aws_region" {
  type        = string
  default     = "us-east-1"
  description = "Region where infra will be deployed"
}

variable "project_name" {
  description = "Prefix used to name/tag all resources"
  type = string
  default = "zero-trust-vehicle-telemetry"
}

variable "aws_account_id" {
  type = string
  description = "AWS account ID"
  sensitive = true
}

variable "table_name" {
  type = string
  description = "DynamoDB table name"
  sensitive = true
}