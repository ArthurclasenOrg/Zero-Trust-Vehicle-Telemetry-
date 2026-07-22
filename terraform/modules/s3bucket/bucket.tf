resource "aws_s3_bucket" "bucket" {
  bucket = "zero-trust-vehicle-telemetry-bucket"
}

# a key to encrypt bucket (changes every 7 days)
resource "aws_kms_key" "kms_key_for_bucket" {
  description = "KMS key for bucket object"
  deletion_window_in_days = 7
}

resource "aws_s3_object" "bucket_object" {
  bucket = aws_s3_bucket.bucket
  key = "vehicle_data"
  kms_key_id = aws_kms_key.kms_key_for_bucket.arn
}


