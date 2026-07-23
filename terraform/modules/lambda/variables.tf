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

variable "aws_account_id" {
  type = string
  description = "AWS ID account"
  sensitive = true
}

variable "project_name" {
  type = string
  description = "Project name"
}