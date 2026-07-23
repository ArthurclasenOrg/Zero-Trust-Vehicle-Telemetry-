resource "aws_cloudtrail" "trail_events" {
  
  name = "trail-events"
  s3_bucket_name = aws_s3_bucket.event_logs.id
  s3_key_prefix = "prefix"

  # monitoring events (infra altered)
  event_selector {
    read_write_type = "All"
    include_management_events = true
    }
}

data "aws_caller_identity" "current" {}

# bucket to store log data
resource "aws_s3_bucket" "event_logs" {
  bucket = "zero-trust-vehicle-telemetry-logs-${data.aws_caller_identity.current.account_id}"
  force_destroy = true
}

# policy for cloudtrail bucket
data "aws_iam_policy_document" "cloudtrail_bucket_policy" {
  statement {
    sid = "AWSCloudTrailAclCheck"
    effect = "Allow"
    principals {
      type = "Service"
      identifiers = ["cloudtrail.amazonaws.com"]
    }
    actions = ["s3:GetBucketAcl"]
    resources = [aws_s3_bucket.event_logs.arn]
  }

  statement {
    sid = "AWSCloudTrailWrite"
    effect = "Allow"
    principals {
      type = "Service"
      identifiers = ["cloudtrail.amazonaws.com"]
    }

    actions = ["s3:PutObject"]
    resources = ["${aws_s3_bucket.event_logs.arn}/prefix/AWSLogs/${data.aws_caller_identity.current.account_id}/*"]
  
    condition {
      test = "StringEquals"
      variable = "s3:x-amz-acl"
      values = ["bucket-owner-full-control"]
    }
  }
}

# attaching the policy to the bucket
resource "aws_s3_bucket_policy" "events_logs_policy" {
  bucket = aws_s3_bucket.event_logs.id
  policy = data.aws_iam_policy_document.cloudtrail_bucket_policy.json
}

# restricting to public
resource "aws_s3_bucket_public_access_block" "event_logs_block" {
  bucket = aws_s3_bucket.event_logs.id
  block_public_acls = true
  block_public_policy = true
  ignore_public_acls = true
  restrict_public_buckets = true
}

# encrypting data 
resource "aws_s3_bucket_server_side_encryption_configuration" "event_logs_encryption" {
  bucket = aws_s3_bucket.event_logs.id
  rule {
    apply_server_side_encryption_by_default {
      sse_algorithm = "AES256"
    }
  }
}
