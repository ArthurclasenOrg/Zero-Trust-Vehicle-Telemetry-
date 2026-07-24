# store-car-service role_id
resource "aws_iam_role" "store_car_svc_role" {
  name = "store_car_svc_role"

  assume_role_policy = jsonencode({
    Version = "2012-10-17"
    Statement = [
      {
        Effect    = "Allow"
        Action    = "sts:AssumeRole"
        Principal = { Service = "lambda.amazonaws.com" }
      }
    ]
  })
}

# defining permissions to store-car-service
resource "aws_iam_role_policy" "store_car_policy" {
    name = "store_car_policy"
    role = aws_iam_role.store_car_svc_role.id

    # policies defined to this service
    policy = jsonencode({
        Version = "2012-10-17"
        Statement = [
            {
                # Storage allowance DynamoDB (store-car only stores data)
                Action = [
                    "dynamodb:PutItem"
                ]
                Effect = "Allow"
                Resource = var.db_state_arn
            },
            {   # using here (/raw) to store the exact data (private and encrypted, limited to this service)
                Sid    = "S3WriteRaw"
                Action = ["s3:PutObject"]
                Effect = "Allow"
                Resource = "${var.bucket_arn}/raw/*"
            },
            {   # using here (/public-history) to write on bucket for public visit
                Sid    = "S3WritePublicHistory"
                Action = ["s3:PutObject"]
                Effect = "Allow"
                Resource = "${var.bucket_arn}/public-history/*"
            },
            {
                # SQS allowance (receive and delete messages + get queue attributes)
                Action = [
                    "sqs:ReceiveMessage",
                    "sqs:DeleteMessage",
                    "sqs:GetQueueAttributes"
                ]
                Effect = "Allow"
                Resource = var.queue_arn
            },
        ]
    })
}

# logging attachment
resource "aws_iam_role_policy_attachment" "store_car_logs" {
  role       = aws_iam_role.store_car_svc_role.name
  policy_arn = "arn:aws:iam::aws:policy/service-role/AWSLambdaBasicExecutionRole"
}

# watch-telemtry role_id
resource "aws_iam_role" "watch_telemetry_role" {
    name = "watch_telemetry_role"
    
    assume_role_policy = jsonencode({
        Version = "2012-10-17"
        Statement = [
            {
                Action = "sts:AssumeRole"
                Effect = "Allow"
                Principal = { Service = "lambda.amazonaws.com" }
            }
        ]
    })
}

# defining permissions to watch-telemtry-service
resource "aws_iam_role_policy" "watch_telemetry_policy" {
    name = "watch_telemetry_policy"
    role = aws_iam_role.watch_telemetry_role.id

    policy = jsonencode({
        Version = "2012-10-17"
        Statement = [
            {
                # Has access to watch last data from each vehicle
                Action = [
                    "dynamodb:GetItem",
                    "dynamodb:Query"
                ]
                Effect = "Allow"
                Resource = [var.db_state_arn,
                            "${var.db_state_arn}/index/*"]
            },
        ]
    })
}

# logging attachment
resource "aws_iam_role_policy_attachment" "watch_telemetry_logs" {
  role       = aws_iam_role.watch_telemetry_role.name
  policy_arn = "arn:aws:iam::aws:policy/service-role/AWSLambdaBasicExecutionRole"
}

# watch-telemtry role_id
resource "aws_iam_role" "public_panel_role" {
    name = "public_panel_role"
    
    assume_role_policy = jsonencode({
        Version = "2012-10-17"
        Statement = [
            {
                Action = "sts:AssumeRole"
                Effect = "Allow"
                Principal = { Service = "lambda.amazonaws.com" }
            }
        ]
    })
}

# defining permissions to public_panel-service
resource "aws_iam_role_policy" "public_panel_policy" {
    name = "public_panel_policy"
    role = aws_iam_role.public_panel_role.id

    policy = jsonencode({
        Version = "2012-10-17"
        Statement = [
            {
                # Has access to watch last data from each vehicle
                Action = [
                    "dynamodb:GetItem",
                    "dynamodb:Query"
                ]
                Effect = "Allow"
                Resource = var.db_public_arn
            },
            {   # using here (/public-history) to expose data to anyone who wants to see the portifolio (pseudonymous data)
                Sid    = "S3ReadPublicHistoryOnly"
                Action = ["s3:GetObject"]
                Effect = "Allow"
                Resource = "${var.bucket_arn}/public-history/*"
            },
                {
                Sid    = "S3ListPublicHistoryOnly"
                Action = ["s3:ListBucket"]
                Effect = "Allow"
                Resource = var.bucket_arn
                Condition = {
                    StringLike = { "s3:prefix" = "public-history/*" }
                }
            }
        ]
    })
}

# logging attachment
resource "aws_iam_role_policy_attachment" "public_panel_logs" {
  role       = aws_iam_role.public_panel_role.name
  policy_arn = "arn:aws:iam::aws:policy/service-role/AWSLambdaBasicExecutionRole"
}

# IoT Core role_id
resource "aws_iam_role" "iot_role" {
  name = "iot_role"

  assume_role_policy = jsonencode({
    Version = "2012-10-17"
    Statement = [
      {
        Effect    = "Allow"
        Action    = "sts:AssumeRole"
        Principal = { Service = "iot.amazonaws.com" }
      }
    ]
  })
}

# permissions for IoT Core service
resource "aws_iam_role_policy" "iot_policy" {
    name = "iot_policy"
    role = aws_iam_role.iot_role.id

    policy = jsonencode({
        Version = "2012-10-17"
        Statement = [
            {
                # Only allowed to put messages on queue
                Action = [
                    "sqs:SendMessage"
                ]
                Effect = "Allow"
                Resource = var.queue_arn
            },
            {
                Effect = "Allow"
                Action = [
                    "logs:CreateLogStream",
                    "logs:PutLogEvents",
                    "logs:DescribeLogGroups",
                    "logs:DescribeLogStreams"
                ]
                Resource = var.cloudwatch_log_group_iot_arn
            }
        ]
    })
}
