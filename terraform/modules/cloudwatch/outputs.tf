output "cloudwatch_log_group_iot_arn" {
  value = aws_cloudwatch_log_group.iot_logs.arn
}

output "cloudwatch_log_group_public_panel_svc" {
  value = aws_cloudwatch_log_group.public_panel_svc_logs.arn
}

output "cloudwatch_log_group_watch_telemetry_svc" {
  value = aws_cloudwatch_log_group.watch_telemetry_svc_logs.arn
}

output "cloudwatch_log_group_store_car_svc" {
  value = aws_cloudwatch_log_group.store_car_svc_logs.arn 
}