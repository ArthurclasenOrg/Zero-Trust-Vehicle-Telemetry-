variable "project_name" {
  type = string
  description = "Prefix used to tag/scope sqs"
}

variable "aws_region" {
  type = string
  description = "aws region used for referencing queue"
}