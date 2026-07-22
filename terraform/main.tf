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

