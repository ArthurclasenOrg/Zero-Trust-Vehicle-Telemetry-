# sqs queue
resource "aws_sqs_queue" "data_queue" {
  name = "data_queue"
  max_message_size = 2048   # 2KB max size message
  message_retention_seconds = 3600 # 1 hour to message to be taken
  # fifo_queue = true # FIFO config
  # content_based_deduplication = true
  sqs_managed_sse_enabled = true # Server-side encryption for queue

  # dealing with fault of messages
  redrive_policy = jsonencode({
    deadLetterTargetArn = aws_sqs_queue.dead_letter_queue.arn
    maxReceiveCount = 5
  })
}

data "aws_caller_identity" "current" {}

# dead letter queue (in case message fails more than 5 times)
resource "aws_sqs_queue" "dead_letter_queue" {
  name = "dead_letter_queue"
  # fifo_queue = true
  sqs_managed_sse_enabled = true
  redrive_allow_policy = jsonencode({
    redrivePermission = "byQueue",
    sourceQueueArns = ["arn:aws:sqs:${var.aws_region}:${data.aws_caller_identity.current.account_id}:data_queue"]
  })
}

