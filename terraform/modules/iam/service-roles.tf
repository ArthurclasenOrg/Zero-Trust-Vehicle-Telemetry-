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
                # Storage allowance (store-car only stores data)
                Action = [
                    "dynamodb:PutItem",
                    "s3:PutObject"
                ]
                Effect = "Allow"
                Resource = "*"
            },
            {
                # SQS allowance (receive and delete messages + get queue attributes)
                Action = [
                    "sqs:ReceiveMessage",
                    "sqs:DeleteMessage",
                    "sqs:GetQueueAttributes"
                ]
                Effect = "Allow"
                Resource = "*"
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
                Resource = "*"
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
                Resource = "*"
            },
            {
                # Bucket permissions (to whoever sees the project can visualize all the data (including history))
                Action = [
                    "s3:GetObject",
                    "s3:ListBucket"
                ]
                Effect = "Allow"
                Resource = "*"
            },
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
                Resource = "*"
            }
        ]
    })
}