output "store_car_svc_role_arn" {
  value = aws_iam_role.store_car_svc_role.arn
}

output "watch_telemetry_role_arn" {
  value = aws_iam_role.watch_telemetry_role.arn
}

output "public_panel_role_arn" {
  value = aws_iam_role.public_panel_role.arn
}

output "iot_role_arn" {
  value = aws_iam_role.iot_role.arn
}