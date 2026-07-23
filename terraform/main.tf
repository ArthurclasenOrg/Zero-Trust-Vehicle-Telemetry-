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
}

module "lambda" {
  source = "./modules/lambda"
  public_panel_role_arn = module.iam.public_panel_role_arn
  store_car_svc_role_arn = module.iam.store_car_svc_role_arn
  watch_telemetry_role_arn = module.iam.watch_telemetry_role_arn
  bucket_arn = module.s3.bucket_arn
  queue_arn = module.sqs.sqs_arn
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