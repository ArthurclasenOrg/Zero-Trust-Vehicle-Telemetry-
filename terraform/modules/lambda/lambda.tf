# lambdas functions (3 services)

# placeholder image for testing
locals {
  placeholder_image = "${var.aws_account_id}.dkr.ecr.us-east-1.amazonaws.com/store-car-svc:latest"
}

# store-car-svc
resource "aws_lambda_function" "store_car_svc" {
  function_name = "store_car_svc"
  role = var.store_car_svc_role_arn
  package_type = "Image" # defining this as a running container image function
  image_uri = local.placeholder_image

  memory_size = 512
  timeout = 30

  architectures = ["x86_64"]

  logging_config {
    log_format            = "JSON"
    application_log_level = "INFO"
    system_log_level      = "WARN"
  }

  tags = {
    Project = var.project_name
  }
}

# evente source mapping for store-car-svc, since it's waiting for get data from queue
resource "aws_lambda_event_source_mapping" "queue_event" {
  function_name = aws_lambda_function.store_car_svc.arn
  event_source_arn = var.queue_arn
  batch_size = 10

  scaling_config {
    maximum_concurrency = 5
  }
}

# watch-telemetry-svc
resource "aws_lambda_function" "watch_telemetry_svc" {
  function_name = "watch_telemetry_svc"
  role = var.watch_telemetry_role_arn
  package_type = "Image" # defining this as a running container image function
  image_uri = local.placeholder_image

  memory_size = 512
  timeout = 30

  architectures = ["x86_64"]

  logging_config {
    log_format            = "JSON"
    application_log_level = "INFO"
    system_log_level      = "WARN"
  }

  tags = {
    Project = var.project_name
  }
}

resource "aws_lambda_function_url" "watch_telemetry_svc_url" {
  function_name = aws_lambda_function.watch_telemetry_svc.arn
  authorization_type = "AWS_IAM"
}

# public-panel-svc
resource "aws_lambda_function" "public_panel_svc" {
  function_name = "public_panel_svc"
  role = var.public_panel_role_arn
  package_type = "Image" # defining this as a running container image function
  image_uri = local.placeholder_image

  memory_size = 512
  timeout = 30

  architectures = ["x86_64"]

  logging_config {
    log_format            = "JSON"
    application_log_level = "INFO"
    system_log_level      = "WARN"
  }

  tags = {
    Project = var.project_name
  }
}

resource "aws_lambda_function_url" "public_panel_svc_url" {
  function_name = aws_lambda_function.public_panel_svc.arn
  authorization_type = "NONE"

  cors {  # allowing CloudFront (only cloudfront) to access public panel svc
    allow_origins = ["https://CLOUDFRONT-DOMAIN"]
    allow_methods = ["GET"]
  }
}