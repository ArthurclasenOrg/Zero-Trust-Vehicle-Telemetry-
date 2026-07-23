data "aws_iot_endpoint" "current" {
  endpoint_type = "iot:Data-ATS"
}

output "iot_endpoint" {
  value = data.aws_iot_endpoint.current.endpoint_address
}