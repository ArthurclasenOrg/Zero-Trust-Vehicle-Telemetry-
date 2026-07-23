# Groups (developers and auditors)
# TODO : I MUST CONFIGURE THE RESOURCES WHEN I DEPLOY OTHER SERVICES (FOR NOW WE HAVE Resource = "*")
resource "aws_iam_group" "developers" {
    name = "developers"
    path = "/users/"
}

resource "aws_iam_group" "auditors" {
    name = "auditors"
    path = "/users/" 
}

resource "aws_iam_group" "cloud_managers" {
    name = "cloud_managers"
    path = "/users/"
}

# developers policies
resource "aws_iam_group_policy" "dev_policies" {
    name = "dev_policies"
    group = aws_iam_group.developers.name

    # policy (only what developers need)
    policy = jsonencode({
        Version = "2012-10-17"
        Statement = [
            {
                Sid = "ReadTelemetryData"
                Action = [
                    "dynamodb:GetItem",
                    "dynamodb:Query",
                    "s3:GetObject",
                    "s3:ListBucket"
                ]
                Effect = "Allow"
                Resource = "*"
                Condition = {
                    StringEquals = {
                        "aws:ResourceTag/Project" = var.project_name
                    }
                }
            },
            {
                Sid = "ReadLogs"
                Action = [
                    "logs:GetLogEvents",
                    "logs:DescribeLogGroups",
                    "logs:FilterLogEvents"
                ]
                Effect = "Allow"
                Resource = "*"
            },
            {
                Sid = "InvokeForTesting"
                Action = [
                    "lambda:InvokeFunction",
                    "lambda:InvokeFunctionUrl",
                    "lambda:GetFunction",
                    "lambda:ListFunctions"
                ]
                Effect = "Allow"
                Resource = "*"
                Condition = {
                    StringEquals = {
                        "aws:ResourceTag/Project" = var.project_name
                    }
                }
            }
        ]
    })
} 

# auditors policies
resource "aws_iam_group_policy" "aud_policies" {
    name = "aud_policies"
    group = aws_iam_group.auditors.name

    # policy (only what auditors need)
    policy = jsonencode({
        Version = "2012-10-17"
        Statement = [
            {
                Sid = "Cloudtrail"
                Action = [
                    "cloudtrail:LookupEvents",
                    "cloudtrail:DescribeTrails", 
                    "cloudtrail:GetEventSelectors",
                ]
                Effect = "Allow"
                Resource = "*"
            },
            {
                Sid = "Cloudwatch"
                Action = [
                    "cloudwatch:DescribeAlarms",
                    "cloudwatch:GetMetricData"
                ]
                Effect = "Allow"
                Resource = "*"
                Condition = {
                    StringEquals = {
                        "aws:ResourceTag/Project" = var.project_name
                    }
                }
            },
            {
                Sid = "Logs"
                Action = [
                    "logs:DescribeLogGroups",
                    "logs:GetLogEvents",
                    "logs:FilterLogEvents"
                ]
                Effect = "Allow"
                Resource = "*"
                Condition = {
                    StringEquals = {
                        "aws:ResourceTag/Project" = var.project_name
                    }
                }
            },
            {
                Sid = "IAMPolicies"
                Action = [
                    "iam:GetPolicy",
                    "iam:GetPolicyVersion",
                    "iam:ListPolicies",
                    "iam:GetRole",
                    "iam:ListRoles",
                    "iam:GetAccountAuthorizationDetails"
                ]
                Effect = "Allow"
                Resource = "*"
            }
        ]
    })
}

# cloud managers policies
resource "aws_iam_group_policy" "man_policies" {
    name = "man_policies"
    group = aws_iam_group.cloud_managers.name

    # policy (only what cloud managers need)
    policy = jsonencode({
        Version = "2012-10-17"
        Statement = [
            {
                Sid = "SSMPermissions"
                Action = [
                    "ssm:GetParameter",
                    "ssm:GetParameters",
                    "ssm:PutParameter",
                    "ssm:DeleteParameter"
                ]
                Effect = "Allow"
                Resource = "*"
            },
            {
                Sid = "ACMPermissions"
                Action = [
                    "acm:ListCertificates",
                    "acm:RequestCertificate",
                    "acm:DeleteCertificate"
                ]
                Effect = "Allow"
                Resource = "*"
            },
            {
                Sid = "IotCorePermissions"
                Action = [
                    "iot:DescribeCertificate",
                    "iot:CreateKeysAndCertificate",
                    "iot:UpdateCertificate",
                    "iot:DeleteCertificate",
                    "iot:CreatePolicy",
                    "iot:AttachPolicy",
                    "iot:DetachPolicy"
                ]
                Effect = "Allow"
                Resource = "*"
            },
            {
                Sid = "KMSPermissions"
                Action = [
                    "kms:CreateKey",
                    "kms:EnableKeyRotation",
                    "kms:ScheduleKeyDeletion"
                ]
                Effect = "Allow"
                Resource = "*"
            },
            {
                Sid = "DescribeUpdateDynamoDBTable"
                Action = [
                    "dynamodb:DescribeTable",
                    "dynamodb:UpdateTable"
                ]
                Effect = "Allow"
                Resource = "*"
            },
            {
                Sid = "GetSetQueueAttributesFromSQS"
                Action = [
                    "sqs:GetQueueAttributes",
                    "sqs:SetQueueAttributes"
                ]
                Effect = "Allow"
                Resource = "*"
            },
            {
                Sid = "LambdaUpdateFuncConfig"
                Action = [
                    "lambda:UpdateFunctionConfiguration"    
                ]
                Effect = "Allow"
                Resource = "*"
            }
        ]
    })
}