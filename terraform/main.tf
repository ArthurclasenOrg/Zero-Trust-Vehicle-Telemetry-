module "s3" {
  source       = "./modules/s3bucket"
  project_name = var.project_name
}

module "iam" {
  source       = "./modules/iam"
  project_name = var.project_name
  bucket_arn   = module.s3.bucket_arn
}

module "dynamodb" {
  source       = "./modules/dynamodb"
  project_name = var.project_name
}