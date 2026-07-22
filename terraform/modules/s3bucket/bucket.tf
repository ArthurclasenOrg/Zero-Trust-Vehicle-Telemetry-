resource "aws_s3_bucket" "bucket" {
  bucket = "${var.project_name}-${data.aws_caller_identity.current.account_id}"
  force_destroy = true # to delete the bucket on terraform destroy even if it has objects inside
  tags = { Project = var.project_name }
}

data "aws_caller_identity" "current" {}

# a key to encrypt bucket (it saves for 7 days in case of terraform destroy)
resource "aws_kms_key" "kms_key_for_bucket" {
  description = "KMS key for bucket object"
  deletion_window_in_days = 7
  enable_key_rotation = true
}

# applying encryption to the bucket
resource "aws_s3_bucket_server_side_encryption_configuration" "bucket_encr_config" {
  bucket = aws_s3_bucket.bucket.id
  rule {
    apply_server_side_encryption_by_default {
      kms_master_key_id = aws_kms_key.kms_key_for_bucket.arn
      sse_algorithm     = "aws:kms"
    }
  }
}

# blocking public access to the bucket
resource "aws_s3_bucket_public_access_block" "block_public" {
  bucket = aws_s3_bucket.bucket.id

  block_public_acls = true
  block_public_policy = true
  ignore_public_acls = true
  restrict_public_buckets = true
}



