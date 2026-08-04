# creating repo on private ECR
resource "aws_ecr_repository" "storage_lambda_repo" {
    name = "storage-service-lambda"
    image_tag_mutability = "MUTABLE"

    # forces terraform to destroy repo in future even if it has images inside
    force_delete = true 
}

# config to cleaning rule (it only mantains the most recent images)
resource "aws_ecr_lifecycle_policy" "storage_repo_cleanup" {
  repository = aws_ecr_repository.storage_lambda_repo.name

  policy = jsonencode({
    rules = [
        {
            rulePriority = 1
            description = "Maintain only the most recent images for free tier"
            selection = {
                tagStatus = "any"
                countType = "imageCountMoreThan"
                countNumber = 3
            }
            action = {
                type = "expire"
            }
        }
    ]
  })
}

# creating repo on private ECR
resource "aws_ecr_repository" "watch_lambda_repo" {
    name = "watch-telemetry-lambda"
    image_tag_mutability = "MUTABLE"

    # forces terraform to destroy repo in future even if it has images inside
    force_delete = true 
}

# config to cleaning rule (it only mantains the most recent images)
resource "aws_ecr_lifecycle_policy" "watch_repo_cleanup" {
  repository = aws_ecr_repository.watch_lambda_repo.name

  policy = jsonencode({
    rules = [
        {
            rulePriority = 1
            description = "Maintain only the most recent images for free tier"
            selection = {
                tagStatus = "any"
                countType = "imageCountMoreThan"
                countNumber = 3
            }
            action = {
                type = "expire"
            }
        }
    ]
  })
}