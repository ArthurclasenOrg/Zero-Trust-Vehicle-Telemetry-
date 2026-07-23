output "sqs_arn" {
  value = aws_sqs_queue.data_queue.arn
}

output "dead_sqs_arn" {
  value = aws_sqs_queue.dead_letter_queue.arn
}

output "sqs_name" {
  value = aws_sqs_queue.data_queue.name
}

output "dead_sqs_name" {
  value = aws_sqs_queue.dead_letter_queue.name
}

output "sqs_url" {
  value = aws_sqs_queue.data_queue.url
}