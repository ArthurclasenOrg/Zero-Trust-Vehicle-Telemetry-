# cloudwatch group for each component

# logs for public panel svc
resource "aws_cloudwatch_log_group" "public_panel_svc_logs" {
  name = "/aws/lambda/${var.public_panel_func_name}"
  retention_in_days = 5

  tags = {
    Project = var.project_name
  }
}

# logs for watch telemetry svc
resource "aws_cloudwatch_log_group" "watch_telemetry_svc_logs" {
  name = "/aws/lambda/${var.watch_telemetry_func_name}"
  retention_in_days = 5

  tags = {
    Project = var.project_name
  }
}

# logs for store car svc
resource "aws_cloudwatch_log_group" "store_car_svc_logs" {
  name = "/aws/lambda/${var.store_car_func_name}"
  retention_in_days = 5

  tags = {
    Project = var.project_name
  }
}

# iot logs group
resource "aws_cloudwatch_log_group" "iot_logs" {
  name = "AWSIotLogsV2"
  retention_in_days = 5

  tags = {
    Project = var.project_name
  }
}