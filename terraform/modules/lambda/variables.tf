variable "store_car_svc_role_arn" {
  type = string
  description = "ARN of store car svc role"
}

variable "watch_telemetry_role_arn" {
  type = string
  description = "ARN of watch telemetry svc role"
}

variable "public_panel_role_arn" {
  type = string
  description = "ARN of public panel svc role"
}

variable "bucket_arn" {
  type = string
  description = "ARN of bucket"
}

variable "queue_arn" {
  type = string
  description = "ARN of sqs"
}

variable "public_panel_svc_logs" {
  type = string
  description = "Lambda func depends on the logs configured"
}

variable "watch_telemetry_svc_logs" {
  type = string
  description = "Lambda func depends on the logs configured"
}

variable "store_car_svc_logs" {
  type = string
  description = "Lambda func depends on the logs configured"
}

# function names for cloudwatch
variable "store_car_func_name" {
  type = string
}

variable "public_panel_func_name" {
  type = string
}

variable "watch_telemetry_func_name" {
  type = string
}
variable "aws_account_id" {
  type = string
  description = "AWS ID account"
  sensitive = true
}

variable "table_name" {
  type = string
  description = "DynamoDB table name"
  sensitive = true
}

variable "project_name" {
  type = string
  description = "Project name"
}

