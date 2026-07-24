# function names for cloudwatch
output "store_car_func_name" {
  value = aws_lambda_function.store_car_svc.function_name
}

output "public_panel_func_name" {
  value = aws_lambda_function.public_panel_svc.function_name
}

output "watch_telemetry_func_name" {
  value = aws_lambda_function.watch_telemetry_svc.function_name
}