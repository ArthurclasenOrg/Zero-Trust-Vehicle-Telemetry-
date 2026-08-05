# lambdas functions (3 services)

# placeholder image for testing
locals {
  placeholder_image_storage = "${var.aws_account_id}.dkr.ecr.us-east-1.amazonaws.com/storage-service-lambda:latest"
  placeholder_image_watch = "${var.aws_account_id}.dkr.ecr.us-east-1.amazonaws.com/watch-telemetry-lambda:latest"
  placeholder_image_public = "${var.aws_account_id}.dkr.ecr.us-east-1.amazonaws.com/public-panel-lambda:latest"
}

# store-car-svc
resource "aws_lambda_function" "store_car_svc" {
  function_name = var.store_car_func_name
  role = var.store_car_svc_role_arn
  package_type = "Image" # defining this as a running container image function
  image_uri = local.placeholder_image_storage

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

  environment {
    variables = {
      BUCKET_NAME = "zero-trust-vehicle-telemetry-${var.aws_account_id}",
      TABLE_NAME = var.table_name
      IS_TEST = "no"
    }
  }

  depends_on = [ var.store_car_svc_logs ]
}

# evente source mapping for store-car-svc, since it's waiting for get data from queue
resource "aws_lambda_event_source_mapping" "queue_event" {
  function_name = aws_lambda_function.store_car_svc.arn
  event_source_arn = var.queue_arn
  batch_size = 10

  scaling_config {
    maximum_concurrency = 5
  }
  enabled = true # TURNING OFF DURING LOCAL TESTS
}

# watch-telemetry-svc
resource "aws_lambda_function" "watch_telemetry_svc" {
  function_name = var.watch_telemetry_func_name
  role = var.watch_telemetry_role_arn
  package_type = "Image" # defining this as a running container image function
  image_uri = local.placeholder_image_watch

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

  environment {
    variables = {
      TABLE_NAME = var.table_name
      IS_TEST = "no"
    }
  }

  depends_on = [ var.watch_telemetry_svc_logs ]
}

resource "aws_lambda_function_url" "watch_telemetry_svc_url" {
  function_name = aws_lambda_function.watch_telemetry_svc.arn
  authorization_type = "AWS_IAM"
}

# public-panel-svc
resource "aws_lambda_function" "public_panel_svc" {
  function_name = var.public_panel_func_name
  role = var.public_panel_role_arn
  package_type = "Image" # defining this as a running container image function
  image_uri = local.placeholder_image_watch

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

  depends_on = [ var.public_panel_svc_logs ]
}

resource "aws_lambda_function_url" "public_panel_svc_url" {
  function_name = aws_lambda_function.public_panel_svc.arn
  authorization_type = "NONE"

  cors {  # allowing CloudFront (only cloudfront) to access public panel svc
    allow_origins = ["*"]
    allow_methods = ["GET"]
  }
}

resource "aws_lambda_permission" "public_panel_url_public_access" {
  statement_id           = "AllowPublicFunctionUrlInvoke"
  action                 = "lambda:InvokeFunctionUrl"
  function_name          = aws_lambda_function.public_panel_svc.function_name
  principal              = "*"
  function_url_auth_type = "NONE"
}

resource "aws_lambda_permission" "public_panel_invoke_function" {
  statement_id  = "AllowPublicInvokeFunction"
  action        = "lambda:InvokeFunction"
  function_name = aws_lambda_function.public_panel_svc.function_name
  principal     = "*"
  # sem condition — o provider ainda não suporta o InvokedViaFunctionUrl (issue aberta no terraform-provider-aws)
}
