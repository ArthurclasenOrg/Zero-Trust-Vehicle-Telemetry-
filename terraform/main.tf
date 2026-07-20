module "iam" {
    source = "./modules/iam"
    project_name = var.project_name
}