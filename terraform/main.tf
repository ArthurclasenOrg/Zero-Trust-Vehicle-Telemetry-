locals {
  store_car_func_name = "store_car_svc"
  watch_telemetry_func_name = "watch_telemetry_svc"
  public_panel_func_name = "public_panel_svc"
}

module "sqs" {
  source = "./modules/sqs"
  project_name = var.project_name
  aws_region = var.aws_region
}

module "s3" {
  source       = "./modules/s3bucket"
  project_name = var.project_name
}

module "dynamodb" {
  source       = "./modules/dynamodb"
  project_name = var.project_name
}

module "iam" {
  source       = "./modules/iam"
  project_name = var.project_name
  bucket_arn   = module.s3.bucket_arn
  queue_arn = module.sqs.sqs_arn
  db_public_arn = module.dynamodb.db_public_arn
  db_state_arn = module.dynamodb.db_state_arn
  cloudwatch_log_group_iot_arn = module.cloudwatch.cloudwatch_log_group_iot_arn
}

module "lambda" {
  source = "./modules/lambda"

  # to define the roles on each func
  public_panel_role_arn = module.iam.public_panel_role_arn
  store_car_svc_role_arn = module.iam.store_car_svc_role_arn
  watch_telemetry_role_arn = module.iam.watch_telemetry_role_arn
  
  # for outputs configs (to what bucket/sqs am I sending)
  bucket_arn = module.s3.bucket_arn
  queue_arn = module.sqs.sqs_arn
  
  # for logging dependency (cloudwatch)
  public_panel_svc_logs = module.cloudwatch.cloudwatch_log_group_public_panel_svc
  watch_telemetry_svc_logs = module.cloudwatch.cloudwatch_log_group_watch_telemetry_svc
  store_car_svc_logs = module.cloudwatch.cloudwatch_log_group_store_car_svc
  
  # function names (cloudwatch = lambda func names)
  public_panel_func_name = local.public_panel_func_name
  store_car_func_name = local.store_car_func_name
  watch_telemetry_func_name = local.watch_telemetry_func_name

  aws_account_id = var.aws_account_id
  project_name = var.project_name
}

module "iotcore" {
  source = "./modules/iotcore"
  iot_role_arn = module.iam.iot_role_arn
  aws_region = var.aws_region
  queue_url = module.sqs.sqs_url
}

module "cloudtrail" {
  source = "./modules/cloudtrail"
}

module "cloudwatch" {
  source = "./modules/cloudwatch"
  project_name = var.project_name
  public_panel_func_name = local.public_panel_func_name
  store_car_func_name = local.store_car_func_name
  watch_telemetry_func_name = local.watch_telemetry_func_name
}

module "cloudfront" {
  source = "./modules/cloudfront"
  public_pannel_svc_function_url = module.lambda.public_panel_svc_function_url
  project_name = var.project_name
}