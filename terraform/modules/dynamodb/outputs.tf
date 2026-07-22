output "db_public_arn" {
  value = aws_dynamodb_table.vehicle-telemetry-public.arn
}

output "db_state_arn" {
  value = aws_dynamodb_table.vehicle-telemetry-state.arn
}

output "db_public_name" {
  value = aws_dynamodb_table.vehicle-telemetry-public.name
}

output "db_state_name" {
  value = aws_dynamodb_table.vehicle-telemetry-state.name
}